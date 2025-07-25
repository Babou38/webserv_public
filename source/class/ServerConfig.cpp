/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 14:32:05 by lechaps           #+#    #+#             */
/*   Updated: 2025/05/16 19:24:31 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

ServerConfig::ServerConfig()
    : autoindex(false), upload_enabled(false) {}

ServerConfig::ServerConfig(const ServerConfig& other)
    : host(other.host), port(other.port), server_name(other.server_name),
      root(other.root), index(other.index), autoindex(other.autoindex),
      upload_enabled(other.upload_enabled), upload_store(other.upload_store),
      cgi(other.cgi), locations(other.locations), methods(other.methods),
      client_max_body_size(other.client_max_body_size), error_pages(other.error_pages) {}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
    if (this != &other)
    {
        host = other.host;
        port = other.port;
        server_name = other.server_name;
        root = other.root;
        index = other.index;
        autoindex = other.autoindex;
        upload_enabled = other.upload_enabled;
        upload_store = other.upload_store;
        cgi = other.cgi;
        locations = other.locations;
        methods = other.methods;
        client_max_body_size = other.client_max_body_size;
        error_pages = other.error_pages;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}
