/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:51:23 by lechaps           #+#    #+#             */
/*   Updated: 2025/06/01 12:36:24 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
HttpResponse handle_post(const HttpRequest& req, const Config& conf) {
    // Vérification que l'upload est configuré
    if (conf.upload_path.empty() || !conf.upload_enabled) {
        HttpResponse res(403, "Forbidden - Upload not configured");
        apply_error_page(res, conf);
        return res;
    }
    
    // Utilise la configuration au lieu de valeurs hardcodées
    mkdir(conf.upload_path.c_str(), 0777);
    std::string path = conf.upload_path + "/upload_result.txt";
    
    std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }
    
    file << req.getBody();
    file.close();
    
    HttpResponse res(200, "OK");
    res.setBody("<html><body><h1>Upload Success!</h1><p>Saved to: " + conf.upload_path + "</p></body></html>");
    return res;
}

/*
//gere la requête POST
HttpResponse handle_post(const HttpRequest& req, const Config& conf)
{
    //verfie l'autorisation d'upload
    if (!conf.upload_enabled)
    {
        HttpResponse res(403, "Forbidden");
        apply_error_page(res, conf);
        return res;
    }

    //crée le dossier d'upload
    mkdir(conf.upload_path.c_str(), 0777);

    //definit le chemin de sauvegarde du fichier uploadé
    std::string path = conf.upload_path + "/upload_result.txt";

    //tente d'ouvrir le fichier en écriture
    std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    //ecrit le corps de la requête dans le fichier
    file << req.getBody();
    file.close();

    //renvoie une réponse HTTP 200 OK avec message HTML
    HttpResponse res(200, "OK");
    res.setBody("<html><body><h1>Upload Success!</h1></body></html>");
    return res;
}*/




