#include "Webserv.hpp"

int main(int argc, char** argv)
{
    std::cout << "=== WebServ - Version finale ===" << std::endl;
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    try {
        std::vector<ServerConfig> servers = loadAndParseConfig(argv[1]);
        
        if (servers.empty()) {
            std::cerr << "Aucune configuration trouvée" << std::endl;
            return 1;
        }
        printConfig(servers); 
        NetworkServer server(servers); 
        if (!server.initialize()) {
            std::cerr << "Erreur d'initialisation" << std::endl;
            return 1;
        }
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}