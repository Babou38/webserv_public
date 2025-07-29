/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 14:44:44 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/25 12:04:13 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//decoupe le contenu du fichier en tokens (mots, ;, {}, etc.)
std::vector<std::string> tokenize(const std::string& content)
{
    std::vector<std::string> tokens;
    std::istringstream iss(content);
    std::string token;
    char c;
    
    while (iss.get(c))
    {
        if (isspace(c)) continue; //on ignore les whitespaces

        else if (c == '#') //on ignore les commentaires
            iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        else if (c == '{' || c == '}' || c == ';')
            tokens.push_back(std::string(1, c)); //si separateur special on ajoute le caractère comme token

        else
        {
            token.clear(); //on reset du token courant
            token += c; //on commence un nouveau token

            //on continue à construire le token tant que le prochain caractère
            // n'est pas un séparateur, un espace ou la fin du stream
            while (iss.peek() != EOF && !isspace(iss.peek()) && iss.peek() != '{' && iss.peek() != '}' && iss.peek() != ';')
                token += static_cast<char>(iss.get());

            tokens.push_back(token); //ajoute le token terminé à la liste
        }
    }
    return tokens; //retourne la liste finale des tokens
}

//recupère le prochain token, ou lève une exception si fin
std::string nextToken(const std::vector<std::string>& tokens, size_t& index)
{
    //si on dépasse la fin du tableau, on lève une erreur
    if (index >= tokens.size()) 
        throw std::runtime_error("Unexpected end of tokens");
    //retourne le token courant puis avance l'index
    return tokens[index++];
}

//consulte le prochain token sans avancer
std::string peekToken(const std::vector<std::string>& tokens, size_t index)
{
    //verifie qu'on est pas à la fin
    if (index >= tokens.size()) 
        throw std::runtime_error("Unexpected end of tokens");
    //retourne le token à l'index courant sans l'incrémenter
    return tokens[index];
}

//attend un token précis, sinon lève une erreur
void expectToken(const std::vector<std::string>& tokens, size_t& index, const std::string& expected)
{
    //si le token à venir ne correspond pas à ce qu'on attend, on lève une erreur
    if (peekToken(tokens, index) != expected)
        throw std::runtime_error("Expected token: " + expected + ", got: " + peekToken(tokens, index));
    //sinon, on passe au token suivant
    ++index;
}

//parse un bloc "server { ... }"
ServerConfig parseServer(const std::vector<std::string>& tokens, size_t& index)
{
    ServerConfig server;

    expectToken(tokens, index, "{"); //on attend le début du bloc server
    while (true)
    {
        std::string tok = nextToken(tokens, index); //recupere le token suivant

        if (tok == "}")
            break; //fin du bloc server

        else if (tok == "listen")
        {
            std::string hostport = nextToken(tokens, index);
            size_t colon = hostport.find(":");
            if (colon != std::string::npos)
            {
                server.host = hostport.substr(0, colon);
                server.port = std::atoi(hostport.substr(colon + 1).c_str());
            }
            else server.port = std::atoi(hostport.c_str());
            expectToken(tokens, index, ";");
        }
        else if (tok == "host")
        { 
            server.host = nextToken(tokens, index);
            expectToken(tokens, index, ";");
        }
        else if (tok == "server_name")
        {
            server.server_name = nextToken(tokens, index);
            expectToken(tokens, index, ";");
        }
        else if (tok == "root")
        {
            server.root = nextToken(tokens, index);
            expectToken(tokens, index, ";"); 
        }
        else if (tok == "index")
        {
            server.index = nextToken(tokens, index);
            expectToken(tokens, index, ";");
        }
        else if (tok == "autoindex")
        { 
            server.autoindex = (nextToken(tokens, index) == "on");
            expectToken(tokens, index, ";");
        }
        else if (tok == "upload_enabled")
        { 
            server.upload_enabled = (nextToken(tokens, index) == "on");
            expectToken(tokens, index, ";");
        }
        else if (tok == "upload_store")
        {
            server.upload_store = nextToken(tokens, index);
            server.upload_enabled = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "cgi")
        { 
            std::string ext = nextToken(tokens, index);
            std::string path = nextToken(tokens, index);
            server.cgi[ext] = path;
            expectToken(tokens, index, ";");
        }
        else if (tok == "client_max_body_size")
        {
            std::string val = nextToken(tokens, index);
            server.client_max_body_size = std::atoi(val.c_str());
            expectToken(tokens, index, ";");
        }
        else if (tok == "error_page")
        {
            std::string code_str = nextToken(tokens, index);
            int code = std::atoi(code_str.c_str());
            std::string uri = nextToken(tokens, index);
            server.error_pages[code] = uri;
            expectToken(tokens, index, ";");
        }
        else if (tok == "methods")
        { 
            while (peekToken(tokens, index) != ";")
                server.methods.push_back(nextToken(tokens, index));
            expectToken(tokens, index, ";");
        }
        else if (tok == "location")
        {
            std::string path = nextToken(tokens, index);
            LocationConfig loc = parseLocation(tokens, index);
            loc.path = path;
            server.locations[path] = loc;
        }
        else throw std::runtime_error("Unknown directive in server block: " + tok);
    }
    return server;
}


//parse un bloc location { ... }
LocationConfig parseLocation(const std::vector<std::string>& tokens, size_t& index)
{
    LocationConfig loc;
    expectToken(tokens, index, "{");
    while (true)
    {
        std::string tok = nextToken(tokens, index);
        if (tok == "}")
            break;
        else if (tok == "root")
        {
            loc.root = nextToken(tokens, index);
            expectToken(tokens, index, ";");
        }
        else if (tok == "index")
        {
            loc.index = nextToken(tokens, index);
            expectToken(tokens, index, ";");
        }
        else if (tok == "autoindex")
        {
            std::string val = nextToken(tokens, index);
            loc.autoindex = (val == "on");
            loc.autoindex_set = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "upload_enabled")
        {
            std::string val = nextToken(tokens, index);
            loc.upload_enabled = (val == "on");
            loc.upload_enabled_set = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "error_page")
        {
            std::string code_str = nextToken(tokens, index);
            int code = std::atoi(code_str.c_str());
            std::string uri = nextToken(tokens, index);
            loc.error_pages[code] = uri;
            expectToken(tokens, index, ";");
        }
        else if (tok == "client_max_body_size")
        {
            std::string val = nextToken(tokens, index);
            loc.client_max_body_size = std::atoi(val.c_str());
            loc.client_max_body_size_set = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "return")
        {
            loc.redirect_to = nextToken(tokens, index);
            loc.has_redirect = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "upload_store")
        {
            loc.upload_store = nextToken(tokens, index);
            loc.upload_enabled = true;
            expectToken(tokens, index, ";");
        }
        else if (tok == "methods")
        {
            loc.methods.clear();
            loc.has_method = true;
            while (peekToken(tokens, index) != ";")
                loc.methods.push_back(nextToken(tokens, index));
            expectToken(tokens, index, ";");
        }
        else if (tok == "cgi")
        {
            std::string ext = nextToken(tokens, index);
            std::string path = nextToken(tokens, index);
            loc.cgi[ext] = path;
            expectToken(tokens, index, ";");
        }
        else
            throw std::runtime_error("Unknown directive in location block: " + tok);
    }
    return loc;
}


//parse un fichier de configuration entier et retourne la liste des serveurs
std::vector<ServerConfig> parseConfigFile(const std::string& filepath)
{
    std::vector<ServerConfig> servers;
    size_t index;
    std::stringstream ss;
    std::vector<std::string> tokens;

    std::ifstream file(filepath.c_str()); //ouvre le fichier en lecture
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + filepath); //verifie ouverture

    ss << file.rdbuf(); //lit tout le contenu dans un stringstream
    tokens = tokenize(ss.str()); //decoupe le contenu en tokens

    
    index = 0;
    while (index < tokens.size()) //tant qu'il reste des tokens
    {
        std::string tok = nextToken(tokens, index); //recupere le prochain token

        //on attend obligatoirement le mot-clé "server" en racine
        if (tok != "server")
            throw std::runtime_error("Expected 'server' keyword at root");

        //parse le bloc server correspondant et ajoute au tableau
        servers.push_back(parseServer(tokens, index));
    }
    return servers; //retourne la liste complète
}

//applique l'héritage des valeurs server -> location
void applyInheritance(ServerConfig& server)
{
    LocationConfig default_loc;

    for (std::map<std::string, LocationConfig>::iterator it = server.locations.begin(); it != server.locations.end(); ++it)
    {
        LocationConfig& loc = it->second;

        if (loc.root.empty()) loc.root = server.root;
        if (loc.index.empty()) loc.index = server.index;
        if (!loc.autoindex_set)
            loc.autoindex = server.autoindex;
        if (!loc.upload_enabled_set)
            loc.upload_enabled = server.upload_enabled;
        for (std::map<int, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
        {
            if (loc.error_pages.find(it->first) == loc.error_pages.end())
                loc.error_pages[it->first] = it->second;
        }
        if(!loc.has_method)
            loc.methods = server.methods;
        if (!loc.client_max_body_size_set)
            loc.client_max_body_size = server.client_max_body_size;
        if (loc.upload_store.empty())
            loc.upload_store = server.upload_store;

        for (std::map<std::string, std::string>::const_iterator cgi_it = server.cgi.begin(); cgi_it != server.cgi.end(); ++cgi_it)
        {
            if (loc.cgi.find(cgi_it->first) == loc.cgi.end())
                loc.cgi[cgi_it->first] = cgi_it->second;
        } 
    }
}

//applique l'héritage à tous les serveurs
void applyAllInheritance(std::vector<ServerConfig>& servers)
{
    for (size_t i = 0; i < servers.size(); ++i)
        applyInheritance(servers[i]);
}


//fonction principale qui charge et parse
std::vector<ServerConfig> loadAndParseConfig(const std::string& filepath)
{
    std::vector<ServerConfig> servers;
    
    servers = parseConfigFile(filepath);
    applyAllInheritance(servers);
    return servers;
}