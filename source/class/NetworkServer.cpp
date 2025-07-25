#include "Webserv.hpp"

Client::Client() : fd(-1), last_active(0), request_complete(false) {}
Client::Client(int f) : fd(f), last_active(std::time(NULL)), request_complete(false) {}




namespace HttpHandler {
    bool is_request_complete(const std::string& buffer) {
        size_t header_end = buffer.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            return false;
        }
        
        size_t content_length_pos = buffer.find("Content-Length:");
        if (content_length_pos != std::string::npos && content_length_pos < header_end) {
            size_t value_start = content_length_pos + 15;
            size_t line_end = buffer.find("\r\n", value_start);
            if (line_end != std::string::npos) {
                std::string length_str = buffer.substr(value_start, line_end - value_start);
                length_str.erase(0, length_str.find_first_not_of(" \t"));
                length_str.erase(length_str.find_last_not_of(" \t") + 1);
                
                int content_length = std::atoi(length_str.c_str());
                size_t body_start = header_end + 4;
                size_t current_body_length = buffer.size() - body_start;
                
                return current_body_length >= static_cast<size_t>(content_length);
            }
        }
        
        return true;
    }
    
    std::string process_request(const std::string& request, const std::vector<ServerConfig>& servers) {
        try {
            HttpRequest req;
            req.parse(request);
            // displayRequest(req);
            req.checkOverrideMethod();
            
            const ServerConfig& server = matchServer(req, servers);
            const LocationConfig* location = matchLocation(req.getUri(), server);

            Config config;
            if (location) {
                config = fromLocation(*location);
            } else {
                config = fromServer(server);
            }

            HttpResponse response = dispatch(req, config);

            if (response.getStatusCode() >= 400) {
                apply_error_page(response, config);
            }
            
            // displayResponse(response);
            return response.toString();
            
        } catch (const std::exception& e) {
            HttpResponse error_response(400, "Bad Request");
            error_response.setHeader("Content-Type", "text/plain");
            error_response.setBody("Bad Request");
            return error_response.toString();
        } catch (...) {
            HttpResponse error_response(500, "Internal Server Error");
            error_response.setHeader("Content-Type", "text/plain");
            error_response.setBody("Internal Server Error");
            return error_response.toString();
        }
    }
}






NetworkServer::NetworkServer(const std::vector<ServerConfig>& srv) : servers(srv), server_fd(-1), epoll_fd(-1) {}
    
NetworkServer::~NetworkServer() {
    cleanup();
}
    
bool NetworkServer::initialize() {
    if (servers.empty()) {
        std::cerr << "Aucune configuration de serveur trouvée" << std::endl;
        return false;
    }
        
    int port = servers[0].port;
        
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Erreur création socket: " << strerror(errno) << std::endl;
        return false;
    }
        
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Erreur setsockopt: " << strerror(errno) << std::endl;
        return false;
    }
        
    if (!set_non_blocking(server_fd)) {
        return false;
    }
        
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        std::cerr << "Erreur epoll_create: " << strerror(errno) << std::endl;
        return false;
    }
        
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        std::cerr << "Erreur epoll_ctl ADD serveur: " << strerror(errno) << std::endl;
        return false;
    }
        
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
        
    if (!servers[0].host.empty() && servers[0].host != "0.0.0.0") {
        if (inet_aton(servers[0].host.c_str(), &addr.sin_addr) == 0) {
            std::cerr << "Adresse IP invalide: " << servers[0].host << std::endl;
            addr.sin_addr.s_addr = INADDR_ANY;
        }
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
        
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Erreur bind port " << port << ": " << strerror(errno) << std::endl;
        return false;
    }
        
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Erreur listen: " << strerror(errno) << std::endl;
        return false;
    }
        
    std::cout << "Serveur démarré sur " << servers[0].host << ":" << port << std::endl;
    return true;
}
    


void NetworkServer::run() {
    std::cout << "Serveur en écoute..." << std::endl;
        
    while (true) {
        struct epoll_event events[64];
        int n_ready = epoll_wait(epoll_fd, events, 64, 1000);
            
        if (n_ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "Erreur epoll_wait: " << strerror(errno) << std::endl;
            break;
        }
            
        cleanup_timeout_clients();
            
        for (int i = 0; i < n_ready; ++i) {
            int fd = events[i].data.fd;
                
            if (fd == server_fd) {
                accept_new_connections();
            } else {
                handle_client_event(fd, events[i].events);
            }
        }
    }
}
    

bool NetworkServer::set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) >= 0;
}
    
void NetworkServer::cleanup() {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        close(it->first);
    }
    clients.clear();
        
    if (epoll_fd >= 0) close(epoll_fd);
    if (server_fd >= 0) close(server_fd);
}
    
void NetworkServer::cleanup_timeout_clients() {
    std::time_t now = std::time(NULL);
    std::vector<int> to_remove;
        
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (now - it->second.last_active > TIMEOUT_SECONDS) {
            to_remove.push_back(it->first);
        }
    }
        
    for (size_t i = 0; i < to_remove.size(); ++i) {
        remove_client(to_remove[i]);
    }
}
    
void NetworkServer::accept_new_connections() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
            
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            break;
        }
            
        if (clients.size() >= MAX_CLIENTS) {
            close(client_fd);
            continue;
        }
            
        if (!set_non_blocking(client_fd)) {
            close(client_fd);
            continue;
        }
            
        struct epoll_event cli_event;
        cli_event.events = EPOLLIN | EPOLLET;
        cli_event.data.fd = client_fd;
            
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &cli_event) < 0) {
            close(client_fd);
            continue;
        }
            
        clients[client_fd] = Client(client_fd);
    }
}
    
void NetworkServer::handle_client_event(int fd, uint32_t events) {
    std::map<int, Client>::iterator client_it = clients.find(fd);
    if (client_it == clients.end()) return;
        
    Client& client = client_it->second;
        
    if (events & (EPOLLERR | EPOLLHUP)) {
        remove_client(fd);
        return;
    }
        
    if (events & EPOLLIN) {
        if (!read_from_client(client)) {
            remove_client(fd);
            return;
        }
            
        if (!client.request_complete && HttpHandler::is_request_complete(client.buffer)) {
            client.request_complete = true;
            process_complete_request(client);
        }
    }
}
    
bool NetworkServer::read_from_client(Client& client) {
    char buffer[4096];
        
    while (true) {
        ssize_t bytes_read = recv(client.fd, buffer, sizeof(buffer) - 1, 0);
            
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            client.buffer += buffer;
            client.last_active = std::time(NULL);
                
            if (client.buffer.size() > MAX_BUFFER_SIZE) {
                return false;
            }
            continue;
        }
        else if (bytes_read == 0) {
            return false;
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return true;
            }
            return false;
        }
    }
}
    
void NetworkServer::process_complete_request(Client& client) {
    std::string response = HttpHandler::process_request(client.buffer, servers);
    send_response(client.fd, response);
    remove_client(client.fd);
}
    
bool NetworkServer::send_response(int fd, const std::string& response) {
    const char* data = response.c_str();
    size_t total = response.size();
    size_t sent = 0;
        
    while (sent < total) {
        ssize_t bytes = send(fd, data + sent, total - sent, MSG_NOSIGNAL);
            
        if (bytes > 0) {
            sent += bytes;
        }
        else if (bytes <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000);
                continue;
            }
            return false;
        }
    }
    return true;
}
    
void NetworkServer::remove_client(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    clients.erase(fd);
}