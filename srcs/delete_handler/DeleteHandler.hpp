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
        int statusCode;
        std::string status_message;
    public:
        DeleteHandler();
        ~DeleteHandler();
        std::string handleDeleteRequest(ParsRequest &request_data,ConfigParser &parser);
        std::vector<std::string> check_root_location(std::string directoryPath);
        std::vector<std::string> listFiles(const std::string& dirPath);
        bool isDirectory(const std::string& path);
        std::vector<std::string> split(const std::string& str, char delim) const;
        std::string generateAttractivePage(const std::string &base_path,int flag);
        std::string getFileExtension(const std::string& filename);
        size_t getFileSize(const std::string& filename);
        void generate_header();
    // // Helper methods
        bool resourceExists(const std::string& path);
        bool deleteFile(const std::string& path);
        bool deleteDirectory(const std::string& path);
    // std::string createResponse(int statusCode, const std::string& message);
};

#endif // DELETE_HANDLER_HPP





// Return appropriate response:

// 200 OK: If the resource was successfully deleted
// 202 Accepted: If the delete request was accepted but not yet processed
// 204 No Content: If the resource was deleted but no content is returned
// 404 Not Found: If the resource doesn't exist
// 403 Forbidden: If the client doesn't have permission to delete
// 500 Internal Server Error: If something went wrong


// DELETE Method Response Codes
// (As you've listed)

// 200 OK: Resource successfully deleted, response includes a representation of the deleted resource
// 202 Accepted: Delete request accepted but not yet completed (for async operations)
// 204 No Content: Resource successfully deleted, no content returned
// 403 Forbidden: Client doesn't have permission to delete the resource
// 404 Not Found: Resource doesn't exist
// 500 Internal Server Error: Server encountered an error

// GET Method Response Codes

// 200 OK: Resource found and returned in the response body
// 204 No Content: Resource exists but has no content to return
// 304 Not Modified: Resource hasn't changed since the last request (when using conditional GET)
// 403 Forbidden: Client doesn't have permission to access the resource
// 404 Not Found: Resource doesn't exist
// 500 Internal Server Error: Server encountered an error