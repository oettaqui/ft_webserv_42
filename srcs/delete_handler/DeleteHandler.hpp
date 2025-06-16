#ifndef DELETE_HANDLER_HPP
#define DELETE_HANDLER_HPP

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
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

#define BUFFER_SIZE_D 1024

class DeleteHandler {
    
    private:
        std::string readFile(const std::string& filePath);
        std::string generateResponse(const std::string& content,ParsRequest &request_data);
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
        std::map<std::string, std::string> contentTypes;
        int client_fd;
        std::string header;
        int check_put_header;
        int check_if;
        int existent_folder;
        int permitions_folder;
        int statusCode;
        std::string status_message;
        size_t ingore_element;
        size_t size;
        size_t totalBytesSent;
        std::ifstream file;
        std::string file_error_path;
        std::string final_res;
        ssize_t contentLength;
        bool is_true_parse;
        bool use_final_res;
    public:
        DeleteHandler();
        ~DeleteHandler();
        std::string handleDeleteRequest(ParsRequest &request_data,ConfigParser &parser);
        std::vector<std::string> check_root_location(std::string directoryPath);
        std::vector<std::string> listFiles(const std::string& dirPath);
        bool isDirectory(const std::string& path);
        std::vector<std::string> split(const std::string& str, char delim) const;
        std::string generateAttractivePage(const std::string &base_path);
        std::string getFileExtension(const std::string& filename);
        size_t getFileSize(const std::string& filename);
        void generate_header();
        std::vector<std::string> get_location_server() const;
        std::string get_path_to_get();
        bool resourceExists(const std::string& path);
        bool deleteFile(const std::string& path);
        bool deleteDirectory(const std::string& path);
        std::string url_decode(std::string url);

        std::string readSmallFile(std::ifstream& file);
        std::string readFile_v2(const std::string& filePath);
        void storeContentTypes(ParsRequest &request_data);
        bool get_is_true_parse() const;
        bool get_use_final_res() const;
};

#endif