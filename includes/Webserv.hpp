/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 12:37:38 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/28 20:26:40 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <set>
#include <dirent.h>
#include <sys/wait.h>
#include <vector>
#include <limits>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <ctime>

#define TIMEOUT_SECONDS 30
#define MAX_BUFFER_SIZE 1048576
#define MAX_CLIENTS 1000

class HttpRequest
{
    public:
        HttpRequest();
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        ~HttpRequest();
    
        void parse(const std::string& raw_request);
    
        const std::string& getMethod() const;
        std::string getUri() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;
        void checkOverrideMethod();
        std::string getFormField(const std::string& key) const;
    
    private:
        std::string method;
        std::string uri;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
};

class HttpResponse
{
    public:
        HttpResponse();
        HttpResponse(int code, const std::string& message);
        HttpResponse(const HttpResponse& other);
        HttpResponse& operator=(const HttpResponse& other);
        ~HttpResponse();
    
        std::string toString() const;

        int getStatusCode() const;
        std::string getStatusMessage() const;
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body_content);
    
    private:
        int status_code;
        std::string status_message;
        std::map<std::string, std::string> headers;
        std::string body;
};


class Config
{
    public:
        std::string host;
        int port;
        std::vector<std::string> server_names;

        std::map<int, std::string> error_pages;
        size_t client_max_body_size;

        std::string root;
        std::string index;
        bool autoindex;

        bool upload_enabled;
        std::string upload_path;

        std::vector<std::string> allowed_methods;

        std::vector<std::string> cgi_extensions;
        std::map<std::string, std::string> cgi_paths;

        std::string redirect_to;
        bool has_redirect;


        Config();
};



// Représente une configuration de location
class LocationConfig 
{
    public:
        std::string path;                  // chemin de la location (ex: /images)
        std::string root;                  // dossier racine de cette location
        bool autoindex;                    // activation/désactivation de l'autoindex
        std::vector<std::string> methods; // méthodes HTTP autorisées
        std::string index;                // fichier d'index par défaut
        bool upload_enabled;              // autorisation de l'upload
        std::string upload_store;         // chemin de stockage des fichiers uploadés
        std::map<std::string, std::string> cgi; // extension -> chemin exécutable cgi
        bool autoindex_set;
        bool upload_enabled_set;
        size_t client_max_body_size;
        bool client_max_body_size_set;
        std::map<int, std::string> error_pages;
        std::string redirect_to;
        bool has_redirect;
        bool has_method;

        LocationConfig();
        LocationConfig(const LocationConfig&);
        LocationConfig& operator=(const LocationConfig&);
        ~LocationConfig();
};
    
// Représente une configuration de serveur
class ServerConfig
{
    public:
        std::string host;                          // adresse IP 
        int port;                                  // port d'écoute
        std::string server_name;                  // nom du serveur
        std::string root;                         // racine du serveur
        std::string index;                        // index global par défaut
        bool autoindex;                           // autoindex par défaut
        bool upload_enabled;                      // upload global autorisé
        std::string upload_store;                 // chemin global de stockage d'upload
        std::map<std::string, std::string> cgi;   // extension -> binaire cgi
        std::map<std::string, LocationConfig> locations; // locations par chemin
        std::vector<std::string> methods; // méthodes HTTP autorisées par défaut
        size_t client_max_body_size;                // taille max du body
        std::map<int, std::string> error_pages;     // liste des page d'erreur

    
        ServerConfig();
        ServerConfig(const ServerConfig&);
        ServerConfig& operator=(const ServerConfig&);
        ~ServerConfig();
};









struct Client {
    int fd;
    std::string buffer;
    std::time_t last_active;
    bool request_complete;
    
    Client();
    Client(int f);
};

class NetworkServer
{ 
    public:
        NetworkServer(const std::vector<ServerConfig>& srv);
        ~NetworkServer();
        bool initialize();
        void run();

    private:
        std::vector<ServerConfig> servers;
        int server_fd;
        int epoll_fd;
        std::map<int, Client> clients;
        
        bool set_non_blocking(int fd);
        void cleanup();
        void cleanup_timeout_clients();
        void accept_new_connections();
        void handle_client_event(int fd, uint32_t events);
        bool read_from_client(Client& client);
        void process_complete_request(Client& client);
        bool send_response(int fd, const std::string& response);
        void remove_client(int fd);
};

namespace HttpHandler {
    bool is_request_complete(const std::string& buffer);
    std::string process_request(const std::string& request, const std::vector<ServerConfig>& servers);
}





//main
void apply_error_page(HttpResponse& res, const Config& conf);
void displayRequest(const HttpRequest& req);
void displayResponse(const HttpResponse& res);
void printConfig(const std::vector<ServerConfig>& servers);

//dispatch
HttpResponse dispatch(const HttpRequest& req, const Config& conf);
Config fromLocation(const LocationConfig& loc);
Config fromServer(const ServerConfig& server);
const LocationConfig* matchLocation(const std::string& uri, const ServerConfig& server);
const ServerConfig& matchServer(const HttpRequest& req, const std::vector<ServerConfig>& servers);

//ConfiParser
std::vector<std::string> tokenize(const std::string& content);
std::string nextToken(const std::vector<std::string>& tokens, size_t& index);
std::string peekToken(const std::vector<std::string>& tokens, size_t index);
void expectToken(const std::vector<std::string>& tokens, size_t& index, const std::string& expected);
ServerConfig parseServer(const std::vector<std::string>& tokens, size_t& index);
LocationConfig parseLocation(const std::vector<std::string>& tokens, size_t& index);
std::vector<ServerConfig> parseConfigFile(const std::string& filepath);
std::vector<ServerConfig> loadAndParseConfig(const std::string& filepath);
void applyInheritance(ServerConfig& server);
void applyAllInheritance(std::vector<ServerConfig>& servers);

//delete
HttpResponse delete_file(const std::string& path, const Config& conf);
HttpResponse delete_directory(const std::string& path, const HttpRequest& req, const Config& conf);
HttpResponse handle_delete(const HttpRequest& req, const Config& conf);
std::string urlDecode(const std::string& str);

//get
HttpResponse serve_file(const std::string& path, const Config& conf);
HttpResponse serve_autoindex(const std::string& dir_path, const Config& conf);
HttpResponse handle_directory_request(const std::string& full_path, const HttpRequest& req, const Config& conf);
HttpResponse handle_get(const HttpRequest& req, const Config& conf);

//post
HttpResponse handle_post(const HttpRequest& req, const Config& conf);

//redirection
HttpResponse handle_redirect(const Config& conf);

//cgi
char** build_cgi_env(const HttpRequest& req, const std::string& script_path);
pid_t launch_cgi(const std::string& interpreter, const std::string& script_path, const HttpRequest& req, int pipefd[2]);
std::string read_cgi_output(int pipefd);
HttpResponse handle_cgi(const HttpRequest& req, const Config& conf);



#endif