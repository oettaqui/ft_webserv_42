#include "GetHandler.hpp"
#include "unistd.h"

std::string GetHandler::handleGetRequest(const std::string& path,ParsRequest &request_data,ConfigParser &parser) {
    std::string content;
    std::string contentType = "text/html";
    Server server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::cout << "***host*** : " << request_data.hostMethod() << " ***port*** : " << request_data.portMethod() << std::endl;
    if(server_socket.getPort() != 0)
        std::cout << "this is the server : ***|" <<  *server_socket.getServerNames().begin() << "|***" << std::endl;
    if (path == "/") {
        content = readFile("public/index.html");
    } 
    else 
    {
        content = readFile(path);
        if (content.empty()) {
            return generateResponse("<h1>404 Not Found</h1>", "text/html");
        }
    }
    return generateResponse(content, contentType);
}

std::string GetHandler::readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    if (!file) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string GetHandler::generateResponse(const std::string& content, const std::string& contentType) {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << content;
    return response.str();
}