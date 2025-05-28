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

#include "../Parse_configfile/ConfigParser.hpp"
#include "../post_handler/PostHandler.hpp"
#include "../get_handler/GetHandler.hpp"
#include "../CGI/CGIPost.hpp"
#include "../CGI/DataCGI.hpp"

class CGIPost;
class PostHandler;
class ParsRequest{
    private:
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
        // add now
        int client_fd;
        std::map<int,std::string>  responses;
        std::vector<std::string> split(const std::string& str, char delim);

        void parseRequestLine(const std::string& line);
        

        PostHandler* postHandler;

        CGIPost *cgiHandler;
        bool CGI;
        int status;

        std::map<std::string, std::string> queryData;


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
        // add now 
        const int& getClientFd() const;
        const std::map<int,std::string>& getResponses() const;

        bool getCGIState() const;
};

#endif