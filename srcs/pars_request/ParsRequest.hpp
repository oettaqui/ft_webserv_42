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

#include "../post_handler/PostHandler.hpp"
#include "../get_handler/GetHandler.hpp"

class ParsRequest{
    private:
        std::string method;
        std::string path;
        std::string version;
        std::string port;
        std::string host;
        std::string requestContent;
        std::map<int,std::string>  responses;
        std::map<std::string, std::string> headers;
        std::string body;
        bool header_parsed;
        bool is_valid;
        bool is_chunked;
        bool is_boundary;
        bool is_Complet;
        std::vector<std::string> split(const std::string& str, char delim);

        void parseRequestLine(const std::string& line);
        

        PostHandler* postHandler;

        // GET
        int is_index;

    public:
        ParsRequest();
        ~ParsRequest();
        void parse(const std::string& request,int client_fd);
        void parseHeaders(const std::string& header_section);
        void printRequest() const;

        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::map<int,std::string>& getResponses() const;
        const std::string& getBody() const;
        bool isValid() const;
        bool isChunked() const;
        bool isBoundary() const;
        const std::string& portMethod() const;
        const std::string& hostMethod() const;
        bool isComplet() const;
        int isIndex() const;
};

#endif