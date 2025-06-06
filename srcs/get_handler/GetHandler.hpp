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
///////////
#include "../CGI/CGI.hpp"
#include "../CGI/DataCGI.hpp"

#define BUFFER_SIZE_G 1024

class CGI;
class ParsRequest;
class GetHandler {
    
    private:
        std::string readFile(const std::string& filePath,ParsRequest &request_data);
        std::string generateResponse(const std::string& content,ParsRequest &request_data);
        std::string trim(const std::string& str, char ch);
        std::vector<std::string> path_location;
        std::string content;
        std::string contentType;
        std::string location_base;
        std::map<std::string, Location>::const_iterator it_find_location_server;
        std::vector<std::string>::iterator it_find_path_location;
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
        int statusCode;
        std::string status_message;
        size_t ingore_element;
        std::ifstream file;
        ssize_t totalBytesSent;
        ssize_t size;
        bool is_true_parse;
        ssize_t contentLength;
        ///
        CGI *cgiHandler;
        bool autoIndex;
        //
        std::string final_res;
        bool cgi_check;
        int cgi_flag;
    public:
        GetHandler();
        ~GetHandler();
        std::string handleGetRequest(ParsRequest &request_data,ConfigParser &parser);
        std::vector<std::string> check_root_location(std::string directoryPath);
        std::vector<std::string> listFiles(const std::string& dirPath);
        bool isDirectory(const std::string& path);
        std::vector<std::string> split(const std::string& str, char delim) const;
        std::string generateAttractivePage(const std::vector<std::string>& items,const std::string &base_path);
        void storeContentTypes(ParsRequest &request_data);
        std::string getFileExtension(const std::string& filename);
        size_t getFileSize(const std::string& filename);
        void generate_header(int flag);
        std::vector<std::string> get_location_server() const;
        std::string get_path_to_get() ;
        bool check_root(const std::string &value_p) const;
        std::string url_decode(std::string url);
        std::string readLargeFileChunked(std::ifstream& file);
        std::string readSmallFile(std::ifstream& file);
        bool get_is_true_parse() const;
        ////
        std::string url_encode_question_marks(std::string url);
        bool isSocketAlive(int sockfd);
        ////
        std::string createRedirectResponse(int statusCode, const std::string& location);
        /////
        bool getCgiCheck() const;

};

#endif