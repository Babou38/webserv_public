/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 11:55:53 by lechaps           #+#    #+#             */
/*   Updated: 2025/06/01 12:37:22 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"Webserv.hpp"

//gere la réponse HTTP pour une redirection configurée dans la config
HttpResponse handle_redirect(const Config& conf)
{
    //crée une réponse 301 Moved Permanently
    HttpResponse res(301, "Moved Permanently");

    //définit l’en-tête "Location" vers l’URL cible indiquée par la configuration
    res.setHeader("Location", conf.redirect_to);

    //construit le corps HTML
    std::stringstream body;
    body << "<html><body>\n";
    body << "<h1>301 Moved Permanently</h1>\n";
    body << "<p>The resource has moved <a href=\"" << conf.redirect_to << "\">here</a>.</p>\n";
    body << "</body></html>\n";

    //attache le corps à la réponse
    res.setBody(body.str());
    return res;
}
