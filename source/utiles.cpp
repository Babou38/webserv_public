#include "Webserv.hpp"

void apply_error_page(HttpResponse& res, const Config& conf)
{
    std::map<int, std::string>::const_iterator it = conf.error_pages.find(res.getStatusCode());
    if (it != conf.error_pages.end())
    {
        std::ifstream file(it->second.c_str());
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            res.setBody(buffer.str());
            return;
        }
    }
    std::stringstream fallback;
    fallback << "<html><body><h1>" << res.getStatusCode() << " - " << res.getStatusMessage() << "</h1></body></html>";
    res.setBody(fallback.str());
}

void displayRequest(const HttpRequest& req)
{
    std::cout << "[Parsed Request]\n";
    std::cout << "Method: " << req.getMethod() << "\n";
    std::cout << "URI: " << req.getUri() << "\n";
    std::cout << "Version: " << req.getVersion() << "\n";
    std::cout << "Headers:\n";
    for (std::map<std::string, std::string>::const_iterator it = req.getHeaders().begin(); it != req.getHeaders().end(); ++it)
        std::cout << "  " << it->first << ": " << it->second << "\n";
    std::cout << "Body: " << req.getBody() << "\n\n";
}

void displayResponse(const HttpResponse& res)
{
    std::cout << "[Generated Response]\n";
    std::cout << res.toString() << "\n";
}

void printConfig(const std::vector<ServerConfig>& servers)
{
    for (size_t i = 0; i < servers.size(); ++i)
    {
        const ServerConfig& s = servers[i];
        std::cout << "=== Server " << i << " ===\n";
        std::cout << "Host: " << s.host << "\n";
        std::cout << "Port: " << s.port << "\n";
        std::cout << "Root: " << s.root << "\n";
        std::cout << "Index: " << s.index << "\n";
        if (s.autoindex)
            std::cout << "Autoindex: on\n";
        else
            std::cout << "Autoindex: off\n";

        if (s.upload_enabled)
            std::cout << "Upload: on\n";
        else
            std::cout << "Upload: off\n";
        std::cout << "Upload store: " << s.upload_store << "\n";
        std::cout << "Client max body size: " << s.client_max_body_size << "\n";

        std::cout << "Error pages:\n";
        for (std::map<int, std::string>::const_iterator it = s.error_pages.begin(); it != s.error_pages.end(); ++it)
            std::cout << "  " << it->first << " => " << it->second << "\n";

        std::cout << "CGI:\n";
        for (std::map<std::string, std::string>::const_iterator it_cgi = s.cgi.begin(); it_cgi != s.cgi.end(); ++it_cgi)
            std::cout << "  " << it_cgi->first << " => " << it_cgi->second << "\n";

        std::cout << "Methods:\n";
        for (size_t j = 0; j < s.methods.size(); ++j)
            std::cout << "  " << s.methods[j] << "\n";

        for (std::map<std::string, LocationConfig>::const_iterator it_loc = s.locations.begin(); it_loc != s.locations.end(); ++it_loc)
        {
            const LocationConfig& loc = it_loc->second;
            std::cout << "  > Location: " << it_loc->first << "\n";
            std::cout << "    Root: " << loc.root << "\n";
            std::cout << "    Index: " << loc.index << "\n";
            if (loc.autoindex)
                std::cout << "    Autoindex: on\n";
            else
                std::cout << "    Autoindex: off\n";

            if (loc.upload_enabled)
                std::cout << "    Upload: on\n";
            else
                std::cout << "    Upload: off\n";
            std::cout << "    Upload store: " << loc.upload_store << "\n";
            std::cout << "    Client max body size: " << loc.client_max_body_size << "\n";

            if (loc.has_redirect)
                std::cout << "    Redirect to: " << loc.redirect_to << "\n";

            std::cout << "    Error pages:\n";
            for (std::map<int, std::string>::const_iterator it = loc.error_pages.begin(); it != loc.error_pages.end(); ++it)
                std::cout << "    " << it->first << " => " << it->second << "\n";

            for (size_t k = 0; k < loc.methods.size(); ++k)
                std::cout << "    Method: " << loc.methods[k] << "\n";

            if (!loc.cgi.empty())
            {
                std::cout << "    CGI:\n";
                for (std::map<std::string, std::string>::const_iterator it_cgi_loc = loc.cgi.begin(); it_cgi_loc != loc.cgi.end(); ++it_cgi_loc)
                    std::cout << "      " << it_cgi_loc->first << " => " << it_cgi_loc->second << "\n";
            }
        }
    }
}