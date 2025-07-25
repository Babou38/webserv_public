/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 14:30:40 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/25 10:46:59 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

LocationConfig::LocationConfig()
    : autoindex(false), upload_enabled(false), autoindex_set(false),
    upload_enabled_set(false), has_redirect(false), has_method(false) {}

LocationConfig::LocationConfig(const LocationConfig& other)
    : path(other.path), root(other.root), autoindex(other.autoindex),
      methods(other.methods), index(other.index),
      upload_enabled(other.upload_enabled), upload_store(other.upload_store),
      cgi(other.cgi), autoindex_set(other.autoindex_set), upload_enabled_set(other.upload_enabled_set),
      client_max_body_size(other.client_max_body_size), client_max_body_size_set(other.client_max_body_size_set),
      error_pages(other.error_pages), redirect_to(other.redirect_to), has_redirect(other.has_redirect),
      has_method(other.has_method){}

LocationConfig& LocationConfig::operator=(const LocationConfig& other)
{
    if (this != &other)
    {
        path = other.path;
        root = other.root;
        autoindex = other.autoindex;
        methods = other.methods;
        index = other.index;
        upload_enabled = other.upload_enabled;
        upload_store = other.upload_store;
        cgi = other.cgi;
        autoindex_set = other.autoindex_set;
        upload_enabled_set = other.upload_enabled_set;
        client_max_body_size = other.client_max_body_size;
        client_max_body_size_set = other.client_max_body_size_set;
        error_pages = other.error_pages;
        redirect_to = other.redirect_to;
        has_redirect = other.has_redirect;
        has_method = other.has_method;
        
    }
    return *this;
}

LocationConfig::~LocationConfig() {}