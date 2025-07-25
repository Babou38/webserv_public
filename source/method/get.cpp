/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:52:44 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/25 10:54:31 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

static std::string detect_mime_type(const std::string& path)
{
    if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos)
        return "text/html";
    if (path.find(".css") != std::string::npos)
        return "text/css";
    if (path.find(".js") != std::string::npos)
        return "application/javascript";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    if (path.find(".png") != std::string::npos)
        return "image/png";
    if (path.find(".gif") != std::string::npos)
        return "image/gif";
    if (path.find(".svg") != std::string::npos)
        return "image/svg+xml";
    if (path.find(".ico") != std::string::npos)
        return "image/x-icon";
    return "application/octet-stream";
}

HttpResponse serve_file(const std::string& path, const Config& conf) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[ERREUR] Impossible d'ouvrir le fichier : " << path << std::endl;
        HttpResponse res(403, "Forbidden");
        apply_error_page(res, conf);
        return res;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "[ERREUR] Lecture du fichier échouée : " << path << std::endl;
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }
    file.close();

    // std::cerr << "[DEBUG] Fichier demandé : " << path << std::endl;

    HttpResponse res(200, "OK");
    res.setHeader("Content-Type", detect_mime_type(path));
    res.setBody(std::string(buffer.begin(), buffer.end()));
    return res;
}

// //sert un fichier statique à partir du chemin donné
// HttpResponse serve_file(const std::string& path, const Config& conf)
// {
//     (void)conf;
    
//     std::ifstream file(path.c_str(), std::ios::binary); // IMPORTANT: mode binaire pour les images
//     if (!file.is_open())
//     {
//         std::cerr << "[ERREUR] Impossible d'ouvrir le fichier : " << path << std::endl;
//         HttpResponse res(403, "Forbidden");
//         apply_error_page(res, conf);
//         return res;
//     }

//     std::cerr << "[DEBUG] Fichier demandé : " << path << std::endl;

//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     file.close();

//     std::string content = buffer.str();

//     HttpResponse res(200, "OK");
    
//     // Déterminer le Content-Type basé sur l'extension
//     std::string content_type = "text/plain";
//     if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos) {
//         content_type = "text/html";
//     } else if (path.find(".css") != std::string::npos) {
//         content_type = "text/css";
//     } else if (path.find(".js") != std::string::npos) {
//         content_type = "application/javascript";
//     } else if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) {
//         content_type = "image/jpeg";
//     } else if (path.find(".png") != std::string::npos) {
//         content_type = "image/png";
//     } else if (path.find(".gif") != std::string::npos) {
//         content_type = "image/gif";
//     } else if (path.find(".svg") != std::string::npos) {
//         content_type = "image/svg+xml";
//     } else if (path.find(".ico") != std::string::npos) {
//         content_type = "image/x-icon";
//     }
    
//     res.setHeader("Content-Type", content_type);
//     res.setBody(content);
//     return res;
// }

/*HttpResponse serve_file(const std::string& path, const Config& conf)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        std::cerr << "[ERREUR] Impossible d'ouvrir le fichier : " << path << std::endl;
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    std::cerr << "[DEBUG] Fichier demandé : " << path << std::endl;

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();
    std::cerr << "[DEBUG] Contenu du fichier (" << content.length() << " bytes) : " << content.substr(0, 100) << "..." << std::endl;

    HttpResponse res(200, "OK");
    
    // Déterminer le Content-Type basé sur l'extension
    std::string content_type = "text/plain";
    if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos) {
        content_type = "text/html";
    } else if (path.find(".css") != std::string::npos) {
        content_type = "text/css";
    } else if (path.find(".js") != std::string::npos) {
        content_type = "application/javascript";
    }
    
    res.setHeader("Content-Type", content_type);
    res.setBody(content);
    return res;
}*/

/*HttpResponse serve_file(const std::string& path, const Config& conf)
{
    //tente d'ouvrir le fichier
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    //lit le contenu du fichier dans un buffer
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    //genere une réponse 200 avec le contenu du fichier
    HttpResponse res(200, "OK");
    res.setBody(buffer.str());
    return res;
}*/


//genere une page HTML listant le contenu d’un répertoire
HttpResponse serve_autoindex(const std::string& dir_path, const Config& conf)
{
    //tente d’ouvrir le répertoire
    DIR* dir = opendir(dir_path.c_str());
    if (!dir)
    {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    //construit une page HTML listant tous les fichiers/dirs
    std::stringstream html;
    html << "<html><body><ul>";

    struct dirent* entry;
    //readdir lit chaque élément contenu dans le dossier fichier, sous-dossier, lien symbolique...
    while ((entry = readdir(dir)) != NULL)
    {
        //d_name est un champ de dirent* contenant le nom de l'entrée
        //crée un lien  vers chaque fichier ou dossier
        html << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
    }

    closedir(dir);
    //termine le HTML proprement
    html << "</ul></body></html>";

    //retourne la page avec 200 OK
    HttpResponse res(200, "OK");
    res.setBody(html.str());
    return res;
}

//gere l'accès à un répertoire (redirige ou sert un index ou autoindex)
HttpResponse handle_directory_request(const std::string& full_path, const HttpRequest& req, const Config& conf)
{
    std::string index_path;
    
    //redirection automatique si l'URI ne finit pas par /
    if (full_path[full_path.size() - 1] != '/')
    {
        HttpResponse res(301, "Moved Permanently");
        res.setHeader("Location", req.getUri() + "/");
        return res;
    }

    //verifie si un fichier index existe
    index_path = full_path + conf.index;
    struct stat st;
    if (stat(index_path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
        return serve_file(index_path, conf);

    //sinon renvoie un autoindex ou une erreur
    if (conf.autoindex)
        return serve_autoindex(full_path, conf);

    HttpResponse res(403, "Forbidden");
    apply_error_page(res, conf);
    return res;
}

//gere la requête GET complete avec support fichiers, répertoires, autoindex
HttpResponse handle_get(const HttpRequest& req, const Config& conf)
{
    //construction du chemin absolu
    std::string full_path;
    struct stat st;
    //stat remplit la structure st avec les métadonnées du fichier ou dossier
    //permet de tester la nature du chemin (fichier, dossier, lien, etc.)

    full_path = conf.root + req.getUri(); 
    //ressource inexistante -> 404 Not Found
    if (stat(full_path.c_str(), &st) != 0)
    {
        HttpResponse res(404, "Not Found");
        apply_error_page(res, conf);
        return res;
    }

    //si répertoire -> logique de gestion des dossiers
    if (S_ISDIR(st.st_mode))
        return handle_directory_request(full_path, req, conf);

    //sinon -> retourne le fichier
    return serve_file(full_path, conf);
}
