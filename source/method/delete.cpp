/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:35:01 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/18 14:49:56 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//supprime un fichier
HttpResponse delete_file(const std::string& path, const Config& conf)
{
    //echec de unlink -> 500
    if (unlink(path.c_str()) != 0)
    {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }
    //succès -> 204 No Content
    return HttpResponse(204, "No Content");
}

//supprime un dossier vide
HttpResponse delete_directory(const std::string& path, const HttpRequest& req, const Config& conf)
{
    //refuse si URI ne finit pas par / -> 409 Conflict
    if (req.getUri().empty() || req.getUri()[req.getUri().size() - 1] != '/')
    {
        HttpResponse res(409, "Conflict");
        apply_error_page(res, conf);
        return res;
    }

    //verifie les permissions d'écriture sur le dossier -> 403 Forbidden si absent
    if (access(path.c_str(), W_OK) != 0)
    {
        HttpResponse res(403, "Forbidden");
        apply_error_page(res, conf);
        return res;
    }

    //tente de supprimer -> 500 si erreur système
    if (rmdir(path.c_str()) != 0)
    {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    //succès -> 204
    return HttpResponse(204, "No Content");
}

std::string urlDecode(const std::string& str) {
    std::string res;
    size_t i = 0;

    while (i < str.length()) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
            res += ch;
            i += 3;
        } else if (str[i] == '+') {
            res += ' ';
            ++i;
        } else {
            res += str[i];
            ++i;
        }
    }
    return res;
}

//gère DELETE complet
HttpResponse handle_delete(const HttpRequest& req, const Config& conf)
{
    std::string path;
    struct stat st;
    //stat remplit la structure st avec les métadonnées du fichier ou dossier
    //permet de tester la nature du chemin (fichier, dossier, lien, etc.)

    std::string uriToDelete = req.getMethod() == "DELETE" && req.getFormField("id") != "" ? req.getFormField("id")
    : req.getUri();
    path = conf.root + "/" + uriToDelete;

    std::cout << path << std::endl;
    //ressource inexistante -> 404 Not Found
    if (stat(path.c_str(), &st) != 0)
    {
        HttpResponse res(404, "Not Found");
        apply_error_page(res, conf);
        return res;
    }

    //fichier -> appelle delete_files
    if (S_ISREG(st.st_mode))
        return delete_file(path, conf);

    //dossier -> appelle delete_directory
    if (S_ISDIR(st.st_mode))
        return delete_directory(path, req, conf);

    //autre type de ressource non pris en charge -> 403 Forbidden
    HttpResponse res(403, "Forbidden");
    
    return res;
}
