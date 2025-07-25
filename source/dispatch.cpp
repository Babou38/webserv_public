/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 10:41:15 by lechaps           #+#    #+#             */
/*   Updated: 2025/06/01 14:37:24 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//fonction qui aiguille en fonction de la methode
HttpResponse dispatch(const HttpRequest& req, const Config& conf)
{
    std::string method;
    std::string path;
    std::string extension;

    //verifie la taille du body (sert a eviter les crash askip)
    if (req.getBody().size() > conf.client_max_body_size)
    {
        HttpResponse res(413, "Payload Too Large");
        apply_error_page(res, conf);
        return res;
    }
    //vérifie si la méthode est supportée (GET/POST/DELETE uniquement)
    method = req.getMethod();
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        HttpResponse res(501, "Not Implemented");
        apply_error_page(res, conf);
        return res;
    }
    //vérifie si la méthode est autorisée par la config
    if (std::find(conf.allowed_methods.begin(), conf.allowed_methods.end(), req.getMethod()) == conf.allowed_methods.end())
    {
        HttpResponse res(405, "Method Not Allowed");
        apply_error_page(res, conf);
        return res;
    }
    //verifie si redirection
    if (!conf.redirect_to.empty() && conf.has_redirect)
        return handle_redirect(conf);


    // Si le chemin est exécutable, traite en CGI
    path = conf.root + req.getUri();
    // Extraction de l'extension
    std::string::size_type dot_pos = path.find_last_of(".");
    if (dot_pos != std::string::npos)
        extension = path.substr(dot_pos);
    else
        extension = "";
    // Vérifie si l'extension est une extension CGI autorisée
    if (!extension.empty() && std::find(conf.cgi_extensions.begin(), conf.cgi_extensions.end(), extension) != conf.cgi_extensions.end())
        return handle_cgi(req, conf);

    //redirige vers la méthode correspondante
    if (method == "POST")
        return handle_post(req, conf);
    else if (method == "GET")
        return handle_get(req, conf);
    else // DELETE
        return handle_delete(req, conf);
}

//convertie une class locationconfig en class config unie
Config fromLocation(const LocationConfig& loc)
{
    Config conf;

    conf.root = loc.root;
    conf.index = loc.index;
    conf.autoindex = loc.autoindex;

    conf.upload_enabled = loc.upload_enabled;
    conf.upload_path = loc.upload_store;

    conf.client_max_body_size = loc.client_max_body_size;
    conf.allowed_methods = loc.methods;
    conf.error_pages = loc.error_pages;
    conf.redirect_to = loc.redirect_to;
    conf.has_redirect = loc.has_redirect;

    conf.cgi_extensions.clear();
    conf.cgi_paths.clear();
    for (std::map<std::string, std::string>::const_iterator it = loc.cgi.begin(); it != loc.cgi.end(); ++it) {
        conf.cgi_extensions.push_back(it->first);
        conf.cgi_paths[it->first] = it->second;
    }
    return conf;
}

//convertie une class serveurconfig en class config unie
Config fromServer(const ServerConfig& server)
{
    Config conf;

    conf.host = server.host;
    conf.port = server.port;
    conf.server_names.clear();
    conf.server_names.push_back(server.server_name);

    conf.root = server.root;
    conf.index = server.index;
    conf.autoindex = server.autoindex;

    conf.upload_enabled = server.upload_enabled;
    conf.upload_path = server.upload_store;

    conf.client_max_body_size = server.client_max_body_size;
    conf.allowed_methods = server.methods;
    conf.error_pages = server.error_pages;

    conf.cgi_extensions.clear();
    conf.cgi_paths.clear();
    for (std::map<std::string, std::string>::const_iterator it = server.cgi.begin(); it != server.cgi.end(); ++it) {
        conf.cgi_extensions.push_back(it->first);
        conf.cgi_paths[it->first] = it->second;
    }
    return conf;
}

//trouve la location concerner si existe
const LocationConfig* matchLocation(const std::string& uri, const ServerConfig& server)
{
    size_t longest;
    const LocationConfig* match = NULL;

    longest = 0;
    for (std::map<std::string, LocationConfig>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it)
    {
        const std::string& prefix = it->first;
        if (uri.compare(0, prefix.size(), prefix) == 0 && prefix.size() >= longest)
        {
            match = &it->second;
            longest = prefix.size();
        }
    }
    return match;
}

//trouve le bon serveur concerner, si pas trouver fallbak sur le premier
const ServerConfig& matchServer(const HttpRequest& req, const std::vector<ServerConfig>& servers)
{
    std::string host_header;
    std::map<std::string, std::string>::const_iterator it = req.getHeaders().find("Host");

    if (it != req.getHeaders().end())
        host_header = it->second;

    for (size_t i = 0; i < servers.size(); ++i)
    {
        std::ostringstream expected;
        expected << servers[i].host << ":" << servers[i].port;

        if (host_header == expected.str())
            return servers[i];
    }

    //fallback
    std::cerr << "[Warning] No matching server for Host: " << host_header << ", using default\n";
    return servers[0];
}

