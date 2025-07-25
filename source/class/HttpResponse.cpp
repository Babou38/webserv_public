/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 18:55:34 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/20 11:25:45 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//constructeur
HttpResponse::HttpResponse() : status_code(200), status_message("OK")
{
    // headers["Content-Type"] = "text/html";
}

HttpResponse::HttpResponse(int code, const std::string& message): status_code(code), status_message(message)
{
    // headers["Content-Type"] = "text/html";
}

HttpResponse::HttpResponse(const HttpResponse& other): status_code(other.status_code), status_message(other.status_message), headers(other.headers), body(other.body) {}

//operateur =
HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
    if (this != &other)
    {
        status_code = other.status_code;
        status_message = other.status_message;
        headers = other.headers;
        body = other.body;
    }
    return *this;
}

//destructeur
HttpResponse::~HttpResponse() {}

//setteur
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& body_content)
{
    body = body_content;
    std::ostringstream oss;
    oss << body.size();
    headers["Content-Length"] = oss.str();
}

//getteur
int HttpResponse::getStatusCode() const
{
    return status_code;
}

std::string HttpResponse::getStatusMessage() const
{
    return status_message;
}



// Dans votre classe HttpResponse, la méthode toString() doit ressembler à ça :
std::string HttpResponse::toString() const
{
    std::stringstream response;
    
    // Ligne de statut HTTP
    response << "HTTP/1.1 " << status_code << " " << status_message << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    
    // Ligne vide pour séparer headers du body
    response << "\r\n";
    
    // Body
    response << body;
    
    return response.str();
}

//focniton qui genere la reponse
/*std::string HttpResponse::toString() const
{
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " " << status_message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << body;
    return response.str();
}*/