#ifndef DATACGI_HPP
#define DATACGI_HPP

#include <string>
#include <map>

struct dataCGI {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> cgiPass;
    std::string file;
    std::string contentType;
    size_t contentLen;
    std::string scriptPath;
    std::string autoIndex;
    std::string CorrectPassCGI;
    std::string queryString;
    std::map<std::string, std::string> headers;
};

#endif