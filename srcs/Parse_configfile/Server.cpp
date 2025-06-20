#include "Server.hpp"
#include <iostream>

Server::Server() : port(0), client_max_body_size(0) {}

Server::~Server() {}


void Server::setHost(const std::string& h) 
{ 
    host = h;
}

void Server::setPort(int p) 
{ 
    port = p;
}

void Server::addServerName(const std::string& name) 
{ 
    server_names.push_back(name);
}

void Server::setClientMaxBodySize(size_t size) 
{ 
    client_max_body_size = size;
}

void Server::addErrorPage(int code, const std::string& page) 
{ 
    error_pages[code] = page;
}

void Server::setRoot(const std::string& r) 
{ 
    root = r;
}

void Server::addLocation(const std::string& path, const Location& loc) { 
    locations[path] = loc;
}


const std::string& Server::getHost() const 
{ 
    return host;
}

int Server::getPort() const 
{ 
    return port;
}

const std::vector<std::string>& Server::getServerNames() const 
{ 
    return server_names;
}

size_t Server::getClientMaxBodySize() const 
{ 
    return client_max_body_size;
}

const std::map<int, std::string>& Server::getErrorPages() const 
{ 
    return error_pages;
}

const std::string& Server::getRoot() const 
{ 
    return root;
}

const std::map<std::string, Location>& Server::getLocations() const 
{ 
    return locations;
}

std::vector<std::string> Server::split(const std::string& str, char delim) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    
    while (getline(stream, token, delim)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool Server::parse_host(const std::string &host_p) const
{
    const char *str = host_p.c_str();
    int check = 0;
    if(strcmp("dump-ubuntu-benguerir",str) == 0 || strcmp("localhost",str) == 0)
        return true;
    else
    {
        if(str[0] == '.')
            return false;
        for (size_t i = 0; i < strlen(str); i++)
        {
            if(str[i] == '.' && check == 0)
                check = 1;
            else if(str[i] == '.' && check == 1)
                return false;
            else if(str[i] != '.')
                check = 0;
            if(!isdigit(str[i]) && str[i] != '.')
                return false;
        }
        if(str[strlen(str) - 1] == '.' || split(host_p,'.').size() != 4)
            return false;
    }
    return true;
    
}

bool Server::isValid() const {
    if (host.empty()) {
        std::cerr << "Error: Host not specified" << std::endl;
        return false;
    }

    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return false;
    }
    if(parse_host(host) == false)
    {
        std::cerr << "Error: Invalid host number" << std::endl;
        return false;  
    }

    if (client_max_body_size == 0) {
        std::cerr << "Error: Client max body size not specified" << std::endl;
        return false;
    }
    for (std::map<std::string, Location>::const_iterator it = locations.begin();
         it != locations.end(); ++it) {
        if (!it->second.isValid())
            return false;
    }

    return true;
}
