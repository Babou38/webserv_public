/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:10:01 by lechaps           #+#    #+#             */
/*   Updated: 2025/07/20 10:57:13 by lechaps          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Webserv.hpp"

// // Construit les variables d'environnement CGI pour execve
// char** build_cgi_env(const HttpRequest& req, const std::string& script_path)
// {
//     std::vector<std::string> env_vec;

//     //variables CGI de base obligatoires
//     env_vec.push_back("REQUEST_METHOD=" + req.getMethod());
//     env_vec.push_back("SCRIPT_FILENAME=" + script_path);
//     env_vec.push_back("SCRIPT_NAME=" + req.getUri());
//     env_vec.push_back("GATEWAY_INTERFACE=CGI/1.1");
//     env_vec.push_back("SERVER_PROTOCOL=" + req.getVersion());
//     env_vec.push_back("REDIRECT_STATUS=200");

//     //variables optionnelles : Content-Length / Content-Type si présentes
//     std::map<std::string, std::string> headers = req.getHeaders();
//     if (headers.count("Content-Length"))
//         env_vec.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
//     if (headers.count("Content-Type"))
//         env_vec.push_back("CONTENT_TYPE=" + headers["Content-Type"]);

//     // QUERY_STRING : partie après le "?" dans l'URI
//     std::string uri = req.getUri();
//     size_t qmark = uri.find("?");
//     if (qmark != std::string::npos)
//         env_vec.push_back("QUERY_STRING=" + uri.substr(qmark + 1));
//     else
//         env_vec.push_back("QUERY_STRING=");

//     //conversion std::vector<std::string> -> char** attendu par execve
//     char** envp = new char*[env_vec.size() + 1];
//     for (size_t i = 0; i < env_vec.size(); ++i)
//         envp[i] = strdup(env_vec[i].c_str());
//     envp[env_vec.size()] = NULL;

//     return envp;
// }


















// // Lance le script CGI avec pipes pour stdin et stdout
// pid_t launch_cgi(const std::string& interpreter, const std::string& script_path, const HttpRequest& req, int stdin_pipe[2], int stdout_pipe[2])
// {
//     std::cout << "\nLaunching CGI: interpreter = " << interpreter << ", script = " << script_path << "\n";

//     pid_t pid = fork();
//     if (pid == 0)
//     {
//         // Enfant : redirige stdin vers le pipe d'entrée
//         close(stdin_pipe[1]);
//         dup2(stdin_pipe[0], STDIN_FILENO);
//         close(stdin_pipe[0]);

//         // Enfant : redirige stdout vers le pipe de sortie
//         close(stdout_pipe[0]);
//         dup2(stdout_pipe[1], STDOUT_FILENO);
//         close(stdout_pipe[1]);

//         //prépare argv et envp pour execve
//         char* argv[] = { (char*)interpreter.c_str(), (char*)script_path.c_str(), NULL };
//         char** envp = build_cgi_env(req, script_path);

//         //exécute l'interpréteur CGI
//         execve(interpreter.c_str(), argv, envp);
//         perror("execve");
//         exit(1);
//     }
//     return pid;
// }












// // Lit la sortie du CGI (stdout)
// std::string read_cgi_output(int pipefd)
// {
//     char buffer[4096]; //jsp quelle valeur mettre en vrai
//     std::stringstream output;
//     ssize_t bytes;
//     while ((bytes = read(pipefd, buffer, sizeof(buffer))) > 0)
//         output.write(buffer, bytes);
//     return output.str();
// }











// // Parse headers CGI depuis la sortie brute
// static void parse_cgi_output(const std::string& output, HttpResponse& res) {
//     size_t header_end = output.find("\r\n\r\n");
//     if (header_end == std::string::npos) {
//         res.setStatus(500, "Bad CGI Output");
//         res.setBody("<html><body><h1>500 - Bad CGI Output</h1></body></html>");
//         return;
//     }

//     std::istringstream stream(output.substr(0, header_end));
//     std::string line;
//     while (std::getline(stream, line)) {
//         if (!line.empty() && line.back() == '\r')
//             line.erase(line.size() - 1);

//         size_t colon = line.find(':');
//         if (colon != std::string::npos) {
//             std::string key = line.substr(0, colon);
//             std::string val = line.substr(colon + 1);
//             while (!val.empty() && (val[0] == ' ' || val[0] == '\t'))
//                 val.erase(0, 1);
//             res.setHeader(key, val);
//         }
//     }

//     std::string body = output.substr(header_end + 4);
//     res.setBody(body);
// }

// HttpResponse handle_cgi(const HttpRequest& req, const Config& conf) {
//     std::string script_path = conf.root + req.getUri();
//     size_t dot_pos = script_path.find_last_of(".");
//     std::string extension = (dot_pos != std::string::npos) ? script_path.substr(dot_pos) : "";

//     if (conf.cgi_paths.find(extension) == conf.cgi_paths.end()) {
//         HttpResponse res(403, "Forbidden");
//         apply_error_page(res, conf);
//         return res;
//     }

//     const std::string& interpreter = conf.cgi_paths.at(extension);
//     int stdin_pipe[2], stdout_pipe[2];
//     if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0) {
//         HttpResponse res(500, "Internal Server Error");
//         apply_error_page(res, conf);
//         return res;
//     }

//     pid_t pid = launch_cgi(interpreter, script_path, req, stdin_pipe, stdout_pipe);
//     if (pid < 0) {
//         HttpResponse res(500, "Internal Server Error");
//         apply_error_page(res, conf);
//         return res;
//     }

//     close(stdin_pipe[0]);
//     if (req.getMethod() == "POST")
//         write(stdin_pipe[1], req.getBody().c_str(), req.getBody().size());
//     close(stdin_pipe[1]);

//     close(stdout_pipe[1]);
//     std::string output = read_cgi_output(stdout_pipe[0]);
//     close(stdout_pipe[0]);
//     waitpid(pid, NULL, 0);

//     HttpResponse res(200, "OK");
//     parse_cgi_output(output, res);
//     return res;
// }




#include "Webserv.hpp"
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Construit les variables d'environnement CGI pour execve
char** build_cgi_env(const HttpRequest& req, const std::string& script_path)
{
    std::vector<std::string> env_vec;
    env_vec.push_back("REQUEST_METHOD=" + req.getMethod());
    env_vec.push_back("SCRIPT_FILENAME=" + script_path);
    env_vec.push_back("SCRIPT_NAME=" + req.getUri());
    env_vec.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env_vec.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env_vec.push_back("REDIRECT_STATUS=200");

    std::map<std::string, std::string> headers = req.getHeaders();
    if (headers.count("Content-Length"))
        env_vec.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
    if (headers.count("Content-Type"))
        env_vec.push_back("CONTENT_TYPE=" + headers["Content-Type"]);

    std::string uri = req.getUri();
    size_t qmark = uri.find("?");
    env_vec.push_back("QUERY_STRING=" + (qmark != std::string::npos ? uri.substr(qmark + 1) : ""));

    char** envp = new char*[env_vec.size() + 1];
    for (size_t i = 0; i < env_vec.size(); ++i)
        envp[i] = strdup(env_vec[i].c_str());
    envp[env_vec.size()] = NULL;

    return envp;
}

pid_t launch_cgi(const std::string& interpreter, const std::string& script_path, const HttpRequest& req, int stdin_pipe[2], int stdout_pipe[2])
{
    std::cout << "\nLaunching CGI: interpreter = " << interpreter << ", script = " << script_path << "\n";

    pid_t pid = fork();
    if (pid == 0)
    {
        close(stdin_pipe[1]);
        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        close(stdout_pipe[0]);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[1]);

        char* argv[] = { (char*)interpreter.c_str(), (char*)script_path.c_str(), NULL };
        char** envp = build_cgi_env(req, script_path);

        execve(interpreter.c_str(), argv, envp);
        perror("execve");
        exit(1);
    }
    return pid;
}

std::string read_cgi_output(int pipefd)
{
    char buffer[4096];
    std::stringstream output;
    ssize_t bytes;
    while ((bytes = read(pipefd, buffer, sizeof(buffer))) > 0)
        output.write(buffer, bytes);
    return output.str();
}

void parse_cgi_output(const std::string& output, HttpResponse& res)
{
    std::istringstream iss(output);
    std::string line;
    std::string body;
    bool headers_done = false;

    while (std::getline(iss, line)) {
        if (!line.empty() && *line.rbegin() == '\r')
            line.erase(line.length() - 1);

        if (line.empty()) {
            headers_done = true;
            break;
        }

        size_t sep = line.find(": ");
        if (sep != std::string::npos) {
            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 2);
            res.setHeader(key, val);
        }
    }

    if (!headers_done) {
        res = HttpResponse(500, "Bad CGI Output");
        res.setBody("<html><body><h1>500 - Bad CGI Output</h1></body></html>");
        return;
    }

    body.assign(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>());
    res.setBody(body);
}

HttpResponse handle_cgi(const HttpRequest& req, const Config& conf)
{
    std::string script_path = conf.root + req.getUri();
    std::string extension;
    size_t dot = script_path.find_last_of(".");
    if (dot != std::string::npos)
        extension = script_path.substr(dot);

    if (conf.cgi_paths.find(extension) == conf.cgi_paths.end()) {
        HttpResponse res(403, "Forbidden");
        apply_error_page(res, conf);
        return res;
    }

    const std::string& interpreter = conf.cgi_paths.at(extension);

    int stdin_pipe[2], stdout_pipe[2];
    if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0) {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    pid_t pid = launch_cgi(interpreter, script_path, req, stdin_pipe, stdout_pipe);
    if (pid < 0) {
        HttpResponse res(500, "Internal Server Error");
        apply_error_page(res, conf);
        return res;
    }

    close(stdin_pipe[0]);
    if (req.getMethod() == "POST")
        write(stdin_pipe[1], req.getBody().c_str(), req.getBody().size());
    close(stdin_pipe[1]);

    close(stdout_pipe[1]);
    std::string output = read_cgi_output(stdout_pipe[0]);
    close(stdout_pipe[0]);
    waitpid(pid, NULL, 0);

    HttpResponse res(200, "OK");
    parse_cgi_output(output, res);
    return res;
}