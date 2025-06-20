#ifndef PARSREQUEST_HPP
#define PARSREQUEST_HPP

#include <fstream>
#include <vector>  
#include <map>  
#include <iostream>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#define MAX_CONTENT_LENGTH 10485760 
#define MAX_URL_LENGTH 8192 
#define BUFFER_SIZE_P 1024 

#include "../Parse_configfile/ConfigParser.hpp"
#include "../post_handler/PostHandler.hpp"
#include "../get_handler/GetHandler.hpp"
#include "../delete_handler/DeleteHandler.hpp"
#include "../CGI/CGI.hpp"
#include "../CGI/DataCGI.hpp"
class DeleteHandler;
class GetHandler;
class PostHandler;
class ParsRequest{
    private:
        GetHandler* getHandler;
        DeleteHandler* deleteHandler;
        std::string method;
        std::string path;
        std::string version;
        int port;
        std::string host;
        std::string requestContent;
        std::map<std::string, std::string> headers;
        std::string body;
        bool header_parsed;
        bool is_valid;
        bool is_chunked;
        bool is_boundary;
        bool is_Complet;
        int client_fd;
        std::map<int,std::string>  responses;
        std::vector<std::string> split(const std::string& str, char delim);

        void parseRequestLine(std::string& line);
        

        PostHandler* postHandler;

        CGI *cgiHandler;
        bool Cgi;
        int status;
        std::map<int,std::string> statusMessages;
        std::string query;

        std::map<std::string, std::string> queryData;

        int flagCGI;
        bool FlagRedirect;
        bool flagTimeOUT;

        std::map<int, std::string> statusMap;
        std::map<std::string, std::string> contentTypes_1;
        std::string contentType;
        bool errorFromConfig;
        bool errorReadComplete;
        std::ifstream file_error;
        std::string file_error_path;
        std::string final_res;
        std::vector<Server>::iterator server_it;

        size_t size;
        size_t totalBytesSent;
        bool use_final_res;

    public:
        ParsRequest();
        ~ParsRequest();
        void parse(const std::string& request,int client_fd, ConfigParser &parser);
        void parseHeaders(const std::string& header_section);

        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;
        bool isValid() const;
        bool isChunked() const;
        bool isBoundary() const;
        const int& portMethod() const;
        const std::string& hostMethod() const;
        bool isComplet() const;
        const int& getClientFd() const;
        const std::map<int,std::string>& getResponses() const;
        void setResponses(std::string resp);
        bool getCGIState() const;
        bool getFlagRedirect() const;
        void setFlagRedirect();
        const std::string& getQuery() const;
        
        int getFlagCGI() const;
        bool getFlagTimeOUT() const;
        bool getErrorFromConfig() const;
        void setErrorFromConfig();
        bool getErrorReadComplete() const;
        void setErrorReadComplete();
        bool getFlagParsingHeader() const;
        int getStatus() const;
        void trim_crlf(std::string &str);
        std::string readSmallFile(std::string filePath);
        std::string getFileExtension(const std::string& filename);
        size_t getFileSize(const std::string& filename);
        std::string normalizePath(const std::string& path);
        bool get_use_final_res() const;
        void set_use_final_res();
    };

#endif