#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <sstream>
#include <iostream>
#include "unistd.h"
#include "cstdlib"
#include "Location.hpp"

class Server {
    private:
        std::string host;
        int port;
        std::vector<std::string> server_names;
        size_t client_max_body_size;
        std::map<int, std::string> error_pages;
        std::string root;
        std::map<std::string, Location> locations;

    public:
        Server();
        ~Server();
        std::vector<std::string> split(const std::string& str, char delim) const;
        void setHost(const std::string& h);
        void setPort(int p);
        void addServerName(const std::string& name);
        void setClientMaxBodySize(size_t size);
        void addErrorPage(int code, const std::string& page);
        void setRoot(const std::string& r);
        void addLocation(const std::string& path, const Location& loc);

        const std::string& getHost() const;
        int getPort() const;
        const std::vector<std::string>& getServerNames() const;
        size_t getClientMaxBodySize() const;
        const std::map<int, std::string>& getErrorPages() const;
        const std::string& getRoot() const;
        const std::map<std::string, Location>& getLocations() const;
        bool isValid() const;
        bool parse_host(const std::string &host_p) const;
};

#endif