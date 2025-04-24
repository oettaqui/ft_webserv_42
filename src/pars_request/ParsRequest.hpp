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

#include "../post_handler/PostHandler.hpp"

class ParsRequest{
    private:
        std::string method;
        std::string path;
        std::string version;
        std::string port;
        std::string host;
        std::string requestContent;
        std::map<std::string, std::string> headers;
        std::string body;
        bool header_parsed;
        bool is_valid;
        bool is_Complet;
        std::vector<std::string> split(const std::string& str, char delim);

        void parseRequestLine(const std::string& line);
        

        PostHandler* postHandler;

    public:
        ParsRequest();
        ~ParsRequest();
        void parse(const std::string& request);
        void parseHeaders(const std::string& header_section);
        void printRequest() const;

        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;
        bool isValid() const;
        const std::string& portMethod() const;
        const std::string& hostMethod() const;
        bool isComplet() const;
};

#endif