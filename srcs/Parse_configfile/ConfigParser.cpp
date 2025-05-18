#include "ConfigParser.hpp"
#include <iostream>
#include <fstream>

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

void ConfigParser::trim(std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first != std::string::npos && last != std::string::npos)
        str = str.substr(first, last - first + 1);
    else
        str = "";
}

bool ConfigParser::parse(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "⛔Error: Cannot open config file⛔: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        if (line == "server {") {
            Server server;
            if (!parseServer(file, server)) {
                file.close();
                return false;
            }
            servers.push_back(server);
        }
        else
            throw "⛔the server should initialize by \'server {\'⛔";
    }

    file.close();
    return validateConfig();
}

bool ConfigParser::parseServer(std::ifstream& file, Server& server) {
    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        if (line == "}")
            return true;
        if (line.find("location ") == 0) {
            if (!parseLocation(file, line, server))
                return false;
            continue;
        }
        if (!parseLine(line, server))
            return false;
    }
    return false;
}

bool ConfigParser::parseLine(const std::string& line, Server& server) {
    size_t pos = line.find(" ");
    if (pos == std::string::npos)
        return false;
    std::string directive = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    if (value[value.length() - 1] == ';')
        value = value.substr(0, value.length() - 1);
    
    trim(value);

    if (directive == "host")
        server.setHost(value);
    else if (directive == "listen")
        server.setPort(atoi(value.c_str()));
    else if (directive == "server_name") {
        size_t start = 0, end;
        while ((end = value.find(" ", start)) != std::string::npos) {
            server.addServerName(value.substr(start, end - start));
            start = end + 1;
        }
        server.addServerName(value.substr(start));
    }
    else if (directive == "client_max_body_size") {
        size_t size = 0;
        char unit = value[value.length() - 1];
        std::string num = value.substr(0, value.length() - 1);
        
        size = atoi(num.c_str());
        if (unit == 'M' || unit == 'm')
            size *= 1024 * 1024;
        else if (unit == 'K' || unit == 'k')
            size *= 1024;
        
        server.setClientMaxBodySize(size);
    }
    else if (directive == "error_page") {
        size_t space_pos = value.find(" ");
        if (space_pos != std::string::npos) {
            int code = atoi(value.substr(0, space_pos).c_str());
            std::string page = value.substr(space_pos + 1);
            server.addErrorPage(code, page);
        }
    }
    return true;
}

bool ConfigParser::parseLocation(std::ifstream& file, const std::string& locationLine, Server& server) {
    // Extract location path
    size_t pathStart = locationLine.find(" ") + 1;
    size_t pathEnd = locationLine.find(" {");
    if (pathEnd == std::string::npos)
        pathEnd = locationLine.find("{");
    
    if (pathEnd == std::string::npos) {
        std::cerr << "Error: Invalid location syntax" << std::endl;
        return false;
    }

    std::string path = locationLine.substr(pathStart, pathEnd - pathStart);
    trim(path);

    Location location;
    location.setPath(path);

    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        
        if (line.empty() || line[0] == '#')
            continue;

        if (line == "}")
            break;

        size_t pos = line.find(" ");
        if (pos == std::string::npos)
            continue;

        std::string directive = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (value[value.length() - 1] == ';')
            value = value.substr(0, value.length() - 1);
        
        trim(value);

        // Parse location directives
        if (directive == "root")
            location.setRoot(value);
        else if (directive == "allow_methods") {
            size_t start = 0, end;
            while ((end = value.find(" ", start)) != std::string::npos) {
                location.addMethod(value.substr(start, end - start));
                start = end + 1;
            }
            location.addMethod(value.substr(start));
        }
        else if (directive == "index") {
            size_t start = 0, end;
            while ((end = value.find(" ", start)) != std::string::npos) {
                location.addIndex(value.substr(start, end - start));
                start = end + 1;
            }
            location.addIndex(value.substr(start));
        }
        else if (directive == "autoindex")
            location.setAutoindex(value == "on");
        else if (directive == "client_max_body_size") {
            size_t size = 0;
            char unit = value[value.length() - 1];
            std::string num = value.substr(0, value.length() - 1);
            
            size = atoi(num.c_str());
            if (unit == 'M' || unit == 'm')
                size *= 1024 * 1024;
            else if (unit == 'K' || unit == 'k')
                size *= 1024;
            
            location.setClientMaxBodySize(size);
        }
        else if (directive == "upload_store")
            location.setUploadStore(value);
        else if (directive == "cgi")
        {
            location.setCgi(value == "on");
        }
        else if(directive == "cgi_pass")
        {
            while(std::getline(file, line))
            {
                trim(line);
                if (line.empty() || line[0] == '#')
                continue;
                if (line == "}")
                    break;
                size_t pos = line.find(" ");
                if (pos == std::string::npos)
                    continue;
                std::string directive = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                if (value[value.length() - 1] == ';')
                    value = value.substr(0, value.length() - 1);  
                trim(value);
                location.addCgiPass(directive,value);
            }
        }
    }

    server.addLocation(path, location);
    return true;
}

bool ConfigParser::validateConfig() const {
    if (servers.empty()) {
        std::cerr << "Error: No servers defined" << std::endl;
        return false;
    }

    for (size_t i = 0; i < servers.size(); ++i) {
        if (!servers[i].isValid() || !servers[i].hasRequiredLocations())
            return false;
    }

    return true;
}

const std::vector<Server>& ConfigParser::getServers() const {
    return servers;
}

const Server& ConfigParser::getServer(const std::string &host_p, const int& port_p) const {
    static Server nullServer;
    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        if(it->getHost() == host_p && it->getPort() == port_p)
            return *it;
        else if(it->getHost() == "localhost"  && host_p == "127.0.0.1" && it->getPort() == port_p)
            return *it;
        else if(it->getHost() == "dump-ubuntu-benguerir"  && host_p == "127.0.1.1" && it->getPort() == port_p)
            return *it;
    }
    return nullServer;
}

void ConfigParser::all_server_data() const {
    for (std::vector<Server>::const_iterator it = this->servers.begin(); it != this->servers.end(); ++it) {
        std::cout << "Server host: " << it->getHost() << std::endl;
        std::cout << "Port: " << it->getPort() << std::endl;
        std::cout << "Client Max Body Size: " << it->getClientMaxBodySize() << std::endl;
        std::cout << "Error Pages: " ;
        for (std::map<int, std::string>::const_iterator iter = it->getErrorPages().begin(); iter != it->getErrorPages().end(); ++iter) {
            std::cout << iter->first << iter->second << std::endl;
        }
        std::cout << "Root: " << it->getRoot() << std::endl;
        std::cout << "Locations : " ;
        for (std::map<std::string, Location>::const_iterator iterl = it->getLocations().begin(); iterl != it->getLocations().end(); ++iterl) {
            std::cout << iterl->first << std::endl;
        }
        std::cout << std::endl; // Print a newline for better readability
    }
}