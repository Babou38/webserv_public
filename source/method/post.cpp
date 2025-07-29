/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:51:23 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/28 23:02:28 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

#include "Webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

// static std::string extract_boundary(const std::string& content_type) {
//     size_t pos = content_type.find("boundary=");
//     if (pos != std::string::npos)
//         return "--" + content_type.substr(pos + 9);  // add leading '--'
//     return "";
// }

// static bool save_multipart_file(const std::string& part, const std::string& upload_path) {
//     size_t name_pos = part.find("filename=\"");
//     if (name_pos == std::string::npos)
//         return false;

//     name_pos += 10;
//     size_t end_name = part.find("\"", name_pos);
//     std::string filename = part.substr(name_pos, end_name - name_pos);

//     size_t header_end = part.find("\r\n\r\n", end_name);
//     if (header_end == std::string::npos)
//         return false;

//     std::string content = part.substr(header_end + 4);
//     if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
//         content = content.substr(0, content.size() - 2);

//     std::string full_path = upload_path + "/" + filename;
//     std::ofstream file(full_path.c_str(), std::ios::binary);
//     if (!file.is_open())
//         return false;
//     file.write(content.c_str(), content.size());
//     return true;
// }

// HttpResponse handle_post(const HttpRequest& req, const Config& conf) {
    
//     std::cerr << "Body size: " << req.getBody().size() << std::endl;

    
//     if (!conf.upload_enabled || conf.upload_path.empty()) {
//         HttpResponse res(403, "Forbidden - Upload not configured");
//         apply_error_page(res, conf);
//         return res;
//     }

//     mkdir(conf.upload_path.c_str(), 0777);

//     const std::string& content_type = req.getHeaders().count("Content-Type") ? req.getHeaders().at("Content-Type") : "";
//     if (content_type.find("multipart/form-data") == std::string::npos) {
//         HttpResponse res(400, "Bad Request - Expected multipart/form-data");
//         apply_error_page(res, conf);
//         return res;
//     }

//     std::string boundary = extract_boundary(content_type);
//     if (boundary.empty()) {
//         HttpResponse res(400, "Bad Request - Missing boundary");
//         apply_error_page(res, conf);
//         return res;
//     }

//     std::stringstream ss(req.getBody());
//     std::string part;
//     int saved = 0;
//     while (std::getline(ss, part, '\n')) {
//         if (part.find(boundary) != std::string::npos) {
//             std::string block;
//             std::getline(ss, block, '\n'); // headers
//             while (std::getline(ss, part, '\n') && part.find(boundary) == std::string::npos)
//                 block += "\n" + part;

//             if (save_multipart_file(block, conf.upload_path))
//                 ++saved;
//         }
//     }

//     std::stringstream sss;
//     sss << saved;
//     HttpResponse res(200, "OK");
//     res.setBody("<html><body><h1>Upload Success!</h1><p>Fichiers enregistres: " + sss.str() + "</p></body></html>");
//     return res;

// }













// HttpResponse handle_post(const HttpRequest& req, const Config& conf) {
//     // Vérification que l'upload est configuré
//     if (conf.upload_path.empty() || !conf.upload_enabled) {
//         HttpResponse res(403, "Forbidden - Upload not configured");
//         apply_error_page(res, conf);
//         return res;
//     }
    
//     // Utilise la configuration au lieu de valeurs hardcodées
//     mkdir(conf.upload_path.c_str(), 0777);
//     std::string path = conf.upload_path + "/upload_result.txt";
    
//     std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);
//     if (!file.is_open()) {
//         HttpResponse res(500, "Internal Server Error");
//         apply_error_page(res, conf);
//         return res;
//     }
    
//     file << req.getBody();
//     file.close();
    
//     HttpResponse res(200, "OK");
//     res.setBody("<html><body><h1>Upload Success!</h1><p>Saved to: " + conf.upload_path + "</p></body></html>");
//     return res;
// }










// --- Fonctions utilitaires ---
static std::string extract_boundary(const std::string& content_type) {
    size_t pos = content_type.find("boundary=");
    if (pos != std::string::npos)
        return "--" + content_type.substr(pos + 9);
    return "";
}

static bool save_multipart_file(const std::string& part, const std::string& upload_path) {
    size_t name_pos = part.find("filename=\"");
    if (name_pos == std::string::npos)
        return false;

    name_pos += 10;
    size_t end_name = part.find("\"", name_pos);
    std::string filename = part.substr(name_pos, end_name - name_pos);

    size_t header_end = part.find("\r\n\r\n", end_name);
    if (header_end == std::string::npos)
        return false;

    std::string content = part.substr(header_end + 4);
    if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
        content = content.substr(0, content.size() - 2);

    std::string full_path = upload_path + "/" + filename;
    std::ofstream file(full_path.c_str(), std::ios::binary);
    if (!file.is_open())
        return false;
    file.write(content.c_str(), content.size());
    return true;
}

HttpResponse handle_post(const HttpRequest& req, const Config& conf) {
    if (!conf.upload_enabled || conf.upload_path.empty()) {
        HttpResponse res(403, "Forbidden - Upload not configured");
        apply_error_page(res, conf);
        return res;
    }

    mkdir(conf.upload_path.c_str(), 0777);

    const std::string& content_type = req.getHeaders().count("Content-Type") ? req.getHeaders().at("Content-Type") : "";

    if (content_type.find("multipart/form-data") != std::string::npos) {
        std::string boundary = extract_boundary(content_type);
        if (boundary.empty()) {
            HttpResponse res(400, "Bad Request - Missing boundary");
            apply_error_page(res, conf);
            return res;
        }

        std::stringstream ss(req.getBody());
        std::string line, buffer;
        int saved = 0;
        while (std::getline(ss, line)) {
            if (line.find(boundary) != std::string::npos) {
                if (!buffer.empty()) {
                    if (save_multipart_file(buffer, conf.upload_path))
                        ++saved;
                    buffer.clear();
                }
            } else {
                buffer += line + "\n";
            }
        }
        if (!buffer.empty() && save_multipart_file(buffer, conf.upload_path)) {
            ++saved;
        }

        std::stringstream sss;
        sss << saved;
        HttpResponse res(200, "OK");
        res.setBody("<html><body><h1>Upload Success!</h1><p>Fichiers enregistres: " + sss.str() + "</p></body></html>");
        return res;

    } else {
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
        res.setBody("<html><body><h1>Upload Success!</h1><p>Saved to: " + path + "</p></body></html>");
        return res;
    }
}