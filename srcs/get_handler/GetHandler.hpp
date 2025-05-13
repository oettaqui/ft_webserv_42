#ifndef GET_HANDLER_HPP
#define GET_HANDLER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
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
#include <iostream>
#include <vector>
#include <dirent.h>   // For opendir, readdir, closedir 
#include <sys/stat.h> // For stat (file information)
#include <algorithm>


class ParsRequest;
class GetHandler {
    
    private:
        std::string readFile(const std::string& filePath);
        std::string generateResponse(const std::string& content, const std::string& contentType);
        std::string trim(const std::string& str, char ch);
        std::vector<std::string> path_location;
        std::string content;
        std::string contentType;
        std::string location_base;
        std::map<std::string, Location>::const_iterator it_find_location_server;
        std::vector<std::string>::const_iterator it_find_path_location;
        Location  location_concerned;
        Server server_socket;
        std::vector<std::string> fileList;
        std::string index_file;
    public:
        GetHandler() {};
        ~GetHandler() {};
        std::string handleGetRequest(ParsRequest &request_data,ConfigParser &parser);
        std::vector<std::string> check_root_location(std::string directoryPath);
        std::vector<std::string> listFiles(const std::string& dirPath);
        bool isDirectory(const std::string& path);
        std::vector<std::string> split(const std::string& str, char delim) const;
        // void ReadyToWork(const std::string& path);
};

#endif