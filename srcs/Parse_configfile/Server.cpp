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

// Validation
bool Server::isValid() const {
    // Check mandatory fields
    if (host.empty()) {
        std::cerr << "Error: Host not specified" << std::endl;
        return false;
    }

    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return false;
    }

    if (client_max_body_size == 0) {
        std::cerr << "Error: Client max body size not specified" << std::endl;
        return false;
    }

    // Validate all locations
    for (std::map<std::string, Location>::const_iterator it = locations.begin();
         it != locations.end(); ++it) {
        if (!it->second.isValid())
            return false;
    }

    return true;
}

bool Server::hasRequiredLocations() const {
    // Check if root location exists
    if (locations.find("/") == locations.end()) {
        std::cerr << "Error: Root location '/' is required" << std::endl;
        return false;
    }
    return true;
}