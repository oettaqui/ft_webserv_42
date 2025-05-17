#include "./ParsRequest.hpp"

ParsRequest::ParsRequest() {
    header_parsed = false;
    is_valid = false;
    is_Complet = false;
    is_chunked = false;
    is_boundary = false;
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
    
        if ((method != "POST" && method != "GET" && method != "DELETE") && (version != "HTTP/1.1" || !path.empty())){
            is_valid = false;
        }
    }else{
        is_valid = false;
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
            std::stringstream ss(parts[1]);
            ss >> port;
        }
    }

    if (headers.find("Host") == headers.end()) {
        is_valid = false; 
    }
    
    
    std::map<std::string, std::string>::const_iterator cl = headers.find("Content-Length");
    if (cl != headers.end()) {
        const std::string& clValue = cl->second;
        
        for (size_t i = 0; i < clValue.length(); i++) {
            if (!isdigit(clValue[i])) {
                is_valid = false;
                break;
            }
        }
        if (is_valid) {
            long contentLength = strtol(clValue.c_str(), NULL, 10);
            if (contentLength < 0) {
                is_valid = false;
            }
        }
    }
    if (headers.find("Content-Type") != headers.end()){
        std::string boundary = "multipart/form-data";

        std::string valueContentType = headers["Content-Type"];

        if (valueContentType.find(boundary) != std::string::npos)
        {
            std::cout << "it is a boundary" << std::endl;
            is_boundary = true;
        }
    }

    if (headers.find("Content-Length") != headers.end() && 
        headers.find("Transfer-Encoding") != headers.end()) {
            std::cout << "hnaaa 1" << std::endl;
            is_valid = false;
    }
    if (headers.find("Content-Length") == headers.end() && 
        headers.find("Transfer-Encoding") != headers.end())
    {
        std::cout << "hnaaa 2" << std::endl;
        std::string check = "chunked";
        if (headers["Transfer-Encoding"] != check) {
            is_valid = false;
        }
        else{
            is_chunked = true;
        }
    }else if (headers.find("Content-Length") == headers.end() && 
    headers.find("Transfer-Encoding") == headers.end()){
        if (method == "POST")
            is_valid = false;
    }

    if (is_boundary && is_chunked)
    {
        std::cout << "Error is boundary and chunked in the same time !!! " << std::endl;
        is_valid = false;
    }
    for (std::map<std::string, std::string>::iterator it = headers.begin(); 
        it != headers.end(); ++it) {
        if (it->second.empty()) {
            is_valid = false;
            std::cout << "in this block " << std::endl;
            break;
        }
    }
}


void ParsRequest::parse(const std::string& request,int client_fd, ConfigParser &parser) {
    this->client_fd = client_fd;
    requestContent += request;
    std::string contentType = "";
    size_t contentLength = 0;
    std::string boundaryValue= "";
    
    
    if (!header_parsed) {
        size_t header_end = requestContent.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            is_valid = false;
            return;
        }
        
        
        header_parsed = true;
        std::string header_section = requestContent.substr(0, header_end);
        body = requestContent.substr(header_end + 4);
        
        std::vector<std::string> lines = split(header_section, '\n');
        if (lines.empty()) {
            is_valid = false;
            return;
        }
        
        parseRequestLine(lines[0]);
        if (!is_valid){
            std::cout << "A BAD REQUEST 1" << std::endl;
            return;
        }
        parseHeaders(header_section);
        if (!is_valid){
            std::cout << "A BAD REQUEST 2" << std::endl;
            return;
        }

        
        if (method == "POST" && !is_chunked && !is_boundary) {
            
            std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
            if (contentTypeIt != headers.end()) {
                contentType = contentTypeIt->second;
            }
            
            std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
            if (contentLengthIt != headers.end()) {
                contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
            }
            
            if (contentLength > 0 ) {
                if (!postHandler)
                {
                    postHandler = new PostHandler();
                }
                postHandler->initialize(*this, parser);
                if (postHandler->getStatus() == 404 || postHandler->getStatus() == 405)
                {
                    std::cout << "ERROR " << std::endl;
                    is_valid = false;
                    is_Complet = true;
                }
                if (postHandler->isRequestComplete()) {
                    std::cout << "true " << std::endl;
                    is_Complet = true;
                }
            }
        } 
        else if (method == "POST" && is_chunked && !is_boundary){
            std::cout << "at the first time is chunked =====  " << std::endl;
            std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
            if (contentTypeIt != headers.end()) {
                contentType = contentTypeIt->second;
            }
            if (!postHandler)
            {
                postHandler = new PostHandler();
            }
            postHandler->initialize(*this, parser);
            if (postHandler->getStatus() == 404 || postHandler->getStatus() == 405)
            {
                std::cout << "ERROR " << std::endl;
                is_valid = false;
                is_Complet = true;
            }
            if (postHandler->isRequestComplete()) {
                    is_valid = false;
                    is_Complet = true;
            }

        }else if (method == "POST" && !is_chunked && is_boundary){

            std::cout << "here i will hendl the boundary " << std::endl;

            std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
            if (contentTypeIt != headers.end()) {
                contentType = contentTypeIt->second;
            }
            // std::cout << contentType << std::endl;
            std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
            if (contentLengthIt != headers.end()) {
                contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
            }
            std::string boundaryPrefix = "boundary=";
            size_t posBoundary = contentType.find(boundaryPrefix);
            // std::cout << "==> " << contentType << std::endl;
            if (posBoundary != std::string::npos)
            {
                size_t startPos = posBoundary + boundaryPrefix.length();
                boundaryValue = contentType.substr(startPos);
                // std::cout << "Boundary value: " << boundaryValue << std::endl;
            }
            if (!postHandler)
            {
                postHandler = new PostHandler();
            }
            // std::cout << "content length " << contentLength << std::endl;
            postHandler->setExpextedLength(contentLength);
            postHandler->initBoundary(body, boundaryValue, *this, parser);
            if (postHandler->getStatus() == 404)
            {
                std::cout << "ERROR 404" << std::endl;
                is_valid = false;
                is_Complet = true;
            }
            if (postHandler->isRequestComplete())
            {
                std::cout << "boundary is complete" << std::endl;
                is_Complet = true;
            }
        }
        else {
            std::cout << "*****non-POST requests" <<std::endl;
            if (method == "GET"){
                GetHandler* getHandler = new GetHandler();
                std::string response = getHandler->handleGetRequest(*this, parser);
                responses[client_fd] = response;
                is_Complet = true;
                delete getHandler;
            }
            is_Complet = true;
        }
    }

    else if (method == "POST" && postHandler) {
        if (is_chunked) {
            // std::cout << "at the second time is chunked =====  " << std::endl;
            postHandler->processChunkedData(request);
            // std::cout << "here continue" << std::endl;
        }
        else if (is_boundary)
        {
            // postHandler->processBoundaryData(body, boundaryValue, *this, parser);
            postHandler->initBoundary(request, boundaryValue, *this, parser);
        }
        else {
            // std::cout << "continue here if the post req is binary " << std::endl;
            postHandler->processData(request);
        }
        
        if (postHandler->isRequestComplete()) {
            std::cout << "complet ++++++++++++\n";
            is_Complet = true;
            // return;
        }
    }


}

const std::string& ParsRequest::getMethod() const { return method; }
const int& ParsRequest::portMethod() const { return port; }
const std::string& ParsRequest::hostMethod() const { return host; }
const std::string& ParsRequest::getPath() const { return path; }
const std::string& ParsRequest::getVersion() const { return version; }
const std::map<std::string, std::string>& ParsRequest::getHeaders() const { return headers; }
const std::string& ParsRequest::getBody() const { return body; }
bool ParsRequest::isValid() const { return is_valid; }
bool ParsRequest::isComplet() const { return is_Complet; }
bool ParsRequest::isChunked() const { return is_chunked; }
bool ParsRequest::isBoundary() const { return is_boundary; }
// add now
const int& ParsRequest::getClientFd() const{return client_fd;}
const std::map<int,std::string>& ParsRequest::getResponses() const { return responses; }

