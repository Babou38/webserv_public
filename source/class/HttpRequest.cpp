/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 12:38:29 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/18 17:57:50 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//constructeur
HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest& other): method(other.method), uri(other.uri), version(other.version), headers(other.headers), body(other.body) {}

//operatuer =
HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
    if (this != &other)
    {
        method = other.method;
        uri = other.uri;
        version = other.version;
        headers = other.headers;
        body = other.body;
    }
    return *this;
}

//destrcuteur
HttpRequest::~HttpRequest() {}

//fonciton qui parse la requete dans une class
void HttpRequest::parse(const std::string& raw_request)
{
    std::size_t header_end;
    std::string header_part;
    std::string body_part;
    std::string line;
    size_t colon_pos;
    std::string key;
    std::string value;
    
    //separe header et body
    header_end = raw_request.find("\r\n\r\n");
    header_part = raw_request.substr(0, header_end);
    body_part = raw_request.substr(header_end + 4);
    
    //parse la request line
    std::istringstream stream(header_part);
    std::getline(stream, line);
    if (!line.empty() && *line.rbegin() == '\r')
        line.erase(line.size() - 1);
    std::istringstream request_line(line);
    request_line >> method >> uri >> version;

    //parse header
    while (std::getline(stream, line))
    {
        if (!line.empty() && *line.rbegin() == '\r')
            line.erase(line.size() - 1);
        colon_pos = line.find(":");
        if (colon_pos != std::string::npos)
        {
            key = line.substr(0, colon_pos);
            value = line.substr(colon_pos + 1);
            value.erase(0, value.find_first_not_of(" "));
            headers[key] = value;
        }
    }
    
    //enregistre body
    body = body_part;
}

std::string HttpRequest::getFormField(const std::string& key) const {
    if (headers.find("Content-Type") == headers.end())
        return "";

    if (headers.at("Content-Type").find("application/x-www-form-urlencoded") == std::string::npos)
        return "";

    std::istringstream stream(body);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            std::string k = pair.substr(0, eq);
            std::string v = pair.substr(eq + 1);
            if (k == key)
                return urlDecode(v);
        }
    }
    return "";
}


void HttpRequest::checkOverrideMethod() {
    if (method != "POST")
        return;   

    std::string contentType = headers["Content-Type"];
    if (contentType.find("application/x-www-form-urlencoded") == std::string::npos)
        return;

    std::istringstream bodyStream(body);
    std::string pair;
    while (std::getline(bodyStream, pair, '&')) {
        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            std::string key = pair.substr(0, eq);
            std::string value = pair.substr(eq + 1);
            if (key == "_method" && (value == "DELETE")) {
                method = value;
                break;
            }
        }
    }
}


//getteur
const std::string& HttpRequest::getMethod() const { return method; }
std::string HttpRequest::getUri() const {
    size_t pos = uri.find('?');
    if (pos != std::string::npos)
        return uri.substr(0, pos);
    return uri;
}
const std::string& HttpRequest::getVersion() const { return version; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return headers; }
const std::string& HttpRequest::getBody() const { return body; }