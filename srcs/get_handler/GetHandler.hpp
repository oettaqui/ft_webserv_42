#ifndef GET_HANDLER_HPP
#define GET_HANDLER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class GetHandler {
public:
    GetHandler() {};
    ~GetHandler() {};

    // Method to handle GET requests
    std::string handleGetRequest(const std::string& path);

private:
    std::string readFile(const std::string& filePath);
    std::string generateResponse(const std::string& content, const std::string& contentType);
};

#endif // GET_HANDLER_HPP