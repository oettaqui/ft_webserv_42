#include "Location.hpp"
#include <iostream>
#include <algorithm>

Location::Location() : autoindex(false), cgi(false), client_max_body_size(0), redirect(false) {}

Location::~Location() {}

void Location::setPath(const std::string& p) 
{
    path = p;
}

void Location::setRoot(const std::string& r)
{
    root = r;
}

void Location::addMethod(const std::string& method) 
{
    methods.push_back(method);
}

void Location::addIndex(const std::string& idx) 
{
    index.push_back(idx);
}

void Location::setAutoindex(bool value) 
{ 
    autoindex = value; 
}
/////////////
void Location::setRedirect() 
{ 
    redirect = true; 
}

bool Location::hasRedirect() const
{
    return redirect;
}

void Location::setRedirection(const int& status, const std::string& path)
{
    redirection[status] = path;
}

const std::map<int, std::string>&  Location::getRedirection() const
{
    return redirection;
}

/////////

void Location::setCgi(bool value) 
{ 
    cgi = value; 
}

void Location::setUploadStore(const std::string& path) 
{ 
    upload_store = path;
}

void Location::setClientMaxBodySize(size_t size) 
{ 
    client_max_body_size = size;
}

void Location::addCgiPass(const std::string& ext, const std::string& handler) {
    cgi_pass[ext] = handler;
}

const std::string& Location::getPath() const 
{ 
    return path;
}

const std::string& Location::getRoot() const 
{ 
    return root;
}

const std::vector<std::string>& Location::getMethods() const 
{ 
    return methods;
}

const std::vector<std::string>& Location::getIndex() const 
{ 
    return index;
}

bool Location::getAutoindex() const 
{ 
    return autoindex;
}

bool Location::getCgi() const 
{ 
    return cgi;
}

const std::string& Location::getUploadStore() const 
{ 
    return upload_store;
}

size_t Location::getClientMaxBodySize() const 
{ 
    return client_max_body_size;
}

const std::map<std::string, std::string>& Location::getCgiPass() const 
{ 
    return cgi_pass;
}

// Validation
bool Location::isValid() const {
    // Check required fields
    if (root.empty()) {
        std::cerr << "Error: Root not specified for location " << path << std::endl;
        return false;
    }
    if (methods.empty()) {
        std::cerr << "Error: No HTTP methods specified for location " << path << std::endl;
        return false;
    }
    // Validate methods
    for (size_t i = 0; i < methods.size(); ++i) {
        if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE") {
            std::cerr << "Error: Invalid HTTP method '" << methods[i] << "'" << std::endl;
            return false;
        }
    }
    // Special checks for CGI location
    if (cgi && cgi_pass.empty()) {
        std::cerr << "Error: CGI enabled but no handlers specified" << std::endl;
        return false;
    }
    // Special checks for upload location
    if (!upload_store.empty() && std::find(methods.begin(), methods.end(), "POST") == methods.end()) {
        std::cerr << "Error: Upload location must allow POST method" << std::endl;
        return false;
    }
    return true;
}