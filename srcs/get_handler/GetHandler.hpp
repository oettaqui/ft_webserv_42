#ifndef GET_HANDLER_HPP
#define GET_HANDLER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <sys/epoll.h>
#include <errno.h>
#include <cstdlib>
#include "../Parse_configfile/ConfigParser.hpp"
#include "../pars_request/ParsRequest.hpp"

class ParsRequest;
class GetHandler {
public:
    GetHandler() {};
    ~GetHandler() {};

    // Method to handle GET requests
    std::string handleGetRequest(const std::string& path,ParsRequest &request_data,ConfigParser &parser);

private:
    std::string readFile(const std::string& filePath);
    std::string generateResponse(const std::string& content, const std::string& contentType);
};

#endif // GET_HANDLER_HPP