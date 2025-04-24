#include "./ParsRequest.hpp"

ParsRequest::ParsRequest() {
    header_parsed = false;
    is_valid = false;
    is_Complet = false;
    postHandler = NULL;
}

ParsRequest::~ParsRequest() {
    if (postHandler) {
        delete postHandler;
    }
}


std::vector<std::string> ParsRequest::split(const std::string& str, char delim) {
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

void ParsRequest::parseRequestLine(const std::string& line) {
    std::vector<std::string> parts = split(line, ' ');
    if (parts.size() == 3) {
        method = parts[0];
        path = parts[1];
        version = parts[2];
        is_valid = true;
    }
}

void ParsRequest::parseHeaders(const std::string& header_section) {
    std::vector<std::string> lines = split(header_section, '\n');
    for (std::vector<std::string>::iterator it = lines.begin(); 
            it != lines.end(); ++it) {
        size_t colon = it->find(':');
        if (colon != std::string::npos) {
            std::string key = it->substr(0, colon);
            std::string value = it->substr(colon + 1);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\r')) {
                value.erase(0, 1);
            }
            while (!value.empty() && 
                    (value[value.size()-1] == ' ' || 
                    value[value.size()-1] == '\r')) {
                value.erase(value.size()-1);
            }
            headers[key] = value;
        }
    }
    std::map<std::string, std::string>::const_iterator it = headers.find("Host");
    if (it != headers.end()) {
        std::vector<std::string> parts = split(it->second, ':');
        if (parts.size() == 2) {
            host = parts[0];
            port = parts[1];
        } else if (parts.size() == 1) {
            host = parts[0];
            port = "80"; // Default HTTP port
        }
    }
}

void ParsRequest::printRequest() const {
    std::cout << "\n=== Request Details ===\n";
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "host: " << host << std::endl;
    
    std::cout << "\n=== Headers ===\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

void ParsRequest::parse(const std::string& request) {
    requestContent += request;
    
    // If headers are not yet parsed, find and parse them
    if (!header_parsed) {
        size_t header_end = requestContent.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            return; // Need more data to find header boundary
        }
        
        // Parse headers
        header_parsed = true;
        std::string header_section = requestContent.substr(0, header_end);
        body = requestContent.substr(header_end + 4);
        
        std::vector<std::string> lines = split(header_section, '\n');
        if (lines.empty()) {
            return;
        }
        
        parseRequestLine(lines[0]);
        parseHeaders(header_section);
        
        // Check if this is a POST request and initialize the handler
        if (method == "POST") {
            std::string contentType = "";
            size_t contentLength = 0;
            
            std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
            if (contentTypeIt != headers.end()) {
                contentType = contentTypeIt->second;
            }
            
            std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
            if (contentLengthIt != headers.end()) {
                contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
            }
            
            if (contentLength > 0) {
                postHandler = new PostHandler();
                postHandler->initialize(contentType, contentLength, body);
                
                // Check if the request is already complete
                if (postHandler->isRequestComplete()) {
                    is_Complet = true;
                }
            }
        } else {
            // For non-POST requests, we're complete after headers are parsed
            is_Complet = true;
        }
    } 
    // If headers already parsed and this is a POST request, pass data to the handler
    else if (method == "POST" && postHandler) {
        postHandler->processData(request);
        
        // Check if we've received all the expected data
        if (postHandler->isRequestComplete()) {
            is_Complet = true;
        }
    }
}

const std::string& ParsRequest::getMethod() const { return method; }
const std::string& ParsRequest::portMethod() const { return port; }
const std::string& ParsRequest::hostMethod() const { return host; }
const std::string& ParsRequest::getPath() const { return path; }
const std::string& ParsRequest::getVersion() const { return version; }
const std::map<std::string, std::string>& ParsRequest::getHeaders() const { return headers; }
const std::string& ParsRequest::getBody() const { return body; }
bool ParsRequest::isValid() const { return is_valid; }
bool ParsRequest::isComplet() const { return is_Complet; }