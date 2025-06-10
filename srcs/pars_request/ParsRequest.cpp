#include "./ParsRequest.hpp"

const std::string HTML_BADREQUEST = 
"HTTP/1.1 400 Bad Request\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n\r\n"
"<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "    <title>not yeeeet</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "    <h1>not yeeee</h1>\r\n"
    "</body>\r\n"
    "</html>\r\n";

ParsRequest::ParsRequest() {
    port = -1;
    header_parsed = false;
    is_valid = false;
    is_Complet = false;
    is_chunked = false;
    is_boundary = false;
    postHandler = NULL;
    getHandler = NULL;
    FlagRedirect = false;
    flagTimeOUT = false;
    this->status = 200;




     statusMap[200] = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Success</title></head>"
                     "<body><h1>Page loaded successfully</h1></body></html>";
    
    statusMap[400] = "HTTP/1.1 400 Bad Request\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Bad Request</title></head>"
                     "<body><h1>Invalid request syntax</h1></body></html>";
    
    statusMap[404] = "HTTP/1.1 404 Not Found\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Not Found</title></head>"
                     "<body><h1>Resource not found</h1></body></html>";
    
    statusMap[405] = "HTTP/1.1 405 Method Not Allowed\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "Allow: GET, POST, DELETE\r\n"
                     "\r\n"
                     "<html><head><title>Method Not Allowed</title></head>"
                     "<body><h1>Method not allowed</h1></body></html>";
    
    statusMap[414] = "HTTP/1.1 414 URI Too Long\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>URI Too Long</title></head>"
                     "<body><h1>Request URI too long</h1></body></html>";
    
    statusMap[500] = "HTTP/1.1 500 Internal Server Error\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Server Error</title></head>"
                     "<body><h1>Internal server error</h1></body></html>";
    
    statusMap[504] = "HTTP/1.1 504 Gateway Timeout\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Gateway Timeout</title></head>"
                     "<body><h1>CGI script timeout</h1></body></html>";
    
    statusMap[408] = "HTTP/1.1 408 Request Timeout\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Request Timeout</title></head>"
                     "<body><h1>Request timeout</h1></body></html>";
    
    statusMap[411] = "HTTP/1.1 411 Length Required\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Length Required</title></head>"
                     "<body><h1>Content-Length required</h1></body></html>";
    
    statusMap[413] = "HTTP/1.1 413 Payload Too Large\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Payload Too Large</title></head>"
                     "<body><h1>Request entity too large</h1></body></html>";
    
    statusMap[501] = "HTTP/1.1 501 Not Implemented\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Not Implemented</title></head>"
                     "<body><h1>Method not implemented</h1></body></html>";

    statusMap[505] = "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Version Not Supported</title></head>"
                     "<body><h1>HTTP version not supported</h1></body></html>";
}

ParsRequest::~ParsRequest() {
    if (postHandler)
        delete postHandler;
    if(getHandler)
        delete getHandler;
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
void ParsRequest::trim_crlf(std::string &str) {
    while (!str.empty() && (str[str.size() - 1] == '\r' || str[str.size() - 1] == '\n')) {
        str.erase(str.size() - 1);
    }
}

// void ParsRequest::parseRequestLine(const std::string& line) {
//     std::vector<std::string> parts = split(line, ' ');
//     if (parts.size() == 3) {
//         method = parts[0];
//         path = parts[1];
//         version = parts[2];
//         trim_crlf(version);
//         is_valid = true;
//         // std::cout << "PATH from parsRequestLine function =========> " << path.length() << std::endl;
//         size_t posQuery = path.find('?');
//         size_t posEndQuery = path.find_last_of('#');
        
//         if(path.length() > MAX_URL_LENGTH)
//         {
//             is_valid = false;
//             header_parsed = false;
//             this->status = 414;
//             return;
//         }
//         if (posQuery != std::string::npos){

//             if (posEndQuery != std::string::npos)
//                 this->query = path.substr(posQuery + 1, posEndQuery);
//             else
//                this->query = path.substr(posQuery + 1, path.length()); 
//             path = path.substr(0, posQuery);

//         }
//         if (version != "HTTP/1.1") {
//             is_valid = false;
//             header_parsed = false;
//             this->status = 505;
//             return;
//         }
//         if ((method != "POST" && method != "GET" && method != "DELETE") || (path.empty())){
//             is_valid = false;
//             this->status = 400;
//             header_parsed = false;
//         }

//     }else{
//         is_valid = false;
//         this->status = 400;
//         header_parsed = false;
//     }
// }

void ParsRequest::parseRequestLine(const std::string& line) {
    std::vector<std::string> parts = split(line, ' ');
 
    if (parts.size() != 3) {
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return;
    }

    method = parts[0];
    path = parts[1];
    version = parts[2];
    trim_crlf(version);
    if (path.length() > MAX_URL_LENGTH) {
        is_valid = false;
        header_parsed = false;
        this->status = 414;
        return;
    }

    if (method != "POST" && method != "GET" && method != "DELETE") {
        is_valid = false;
        header_parsed = false;
        this->status = 405;
        return;
    }
    if (version != "HTTP/1.1") {
        is_valid = false;
        header_parsed = false;
        this->status = 505;
        return;
    }

    if (path.empty()) {
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return;
    }

    size_t posQuery = path.find('?');
    if (posQuery != std::string::npos) {
        size_t posEndQuery = path.find_last_of('#');
        if (posEndQuery != std::string::npos)
            this->query = path.substr(posQuery + 1, posEndQuery);
        else
            this->query = path.substr(posQuery + 1, path.length());
        path = path.substr(0, posQuery);
    }

    is_valid = true;
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
        else
        {
            is_valid = false;
            header_parsed = false;
            this->status = 400;
            return ;
        }
    }else
    {
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return ;
    }
    
    std::map<std::string, std::string>::const_iterator cl = headers.find("Content-Length");
    if (cl != headers.end()) {
        const std::string& clValue = cl->second;
        
        for (size_t i = 0; i < clValue.length(); i++) {
            if (!isdigit(clValue[i])) {
                is_valid = false;
                header_parsed = false;
                this->status = 400;
                return;;
            }
        }
        if (is_valid) {
            long contentLength = strtol(clValue.c_str(), NULL, 10);
            if (contentLength < 0) {
                is_valid = false;
                header_parsed = false;
                this->status = 400;
                return;
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
    }else{
        if (method == "POST")
        {
            std::cout << "content type not found " << std::endl;
            is_valid = false;
            header_parsed = false;
            this->status = 400;
            return;
        }
    }

    if (headers.find("Content-Length") != headers.end() && 
        headers.find("Transfer-Encoding") != headers.end()) {
            if (method == "POST"){
                is_valid = false;
                header_parsed = false;
                this->status = 400;
                return;
            }
    }
    if (headers.find("Content-Length") == headers.end() && 
        headers.find("Transfer-Encoding") != headers.end())
    {
        std::string check = "chunked";
        if (headers["Transfer-Encoding"] != check) {
            if (method == "POST"){
                is_valid = false;
                header_parsed = false;
                this->status = 501;
                return;
            }
        }
        else{
            is_chunked = true;
        }
    }else if (headers.find("Content-Length") == headers.end() && 
    headers.find("Transfer-Encoding") == headers.end()){
        if (method == "POST"){
            is_valid = false;
            header_parsed = false;
            this->status = 411;
            return;
        }
    }

    if (is_boundary && is_chunked)
    {
        std::cout << "Error is boundary and chunked in the same time !!! " << std::endl;
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return;
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

            this->flagTimeOUT = true;
            return;
            
        } else{
            if (this->flagTimeOUT == true)
                this->flagTimeOUT = false;
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

                std::cout << "A BAD REQUEST 1 " << this->status << std::endl;
                this->responses[client_fd] = statusMap[this->status];
                return;
            }
            parseHeaders(header_section);
            if (!is_valid){
                std::cout << "A BAD REQUEST 2 " << this->status << std::endl;
                this->responses[client_fd] = statusMap[this->status];
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
    
                    if(FlagRedirect)
                    {
                        is_Complet = true;
                        return ;   
                    }
                    
                    if (postHandler->getStatus() != 200 )
                    {
                        this->status = postHandler->getStatus();
                        std::cout << "ERROR 1" << std::endl;
                        is_valid = false;
                        is_Complet = true;
                        this->responses[client_fd] = statusMap[this->status];
                        return ;
                    }
                    if (postHandler->isRequestComplete() && postHandler->getCGIState()) {
                        std::cout << "is a CGI and parsRequest Complete\n";
                            cgiHandler =  new CGI();
                            dataCGI data;
                            data.method = method;
                            data.path = path;
                            data.version = version;
                            data.file = postHandler->getFilename();
                            data.contentType = postHandler->getContentType();
                            data.contentLen = postHandler->getCurrentLength();
                            data.scriptPath = postHandler->getScriptPath();
                            data.queryString = this->query;
                            if (postHandler->getAutoindexFromPost())
                                data.autoIndex = "true";
                            else
                                data.autoIndex = "false";
                            std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                            std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + postHandler->getExtension());
                            if (passCGIIT != passCGI.end()){
                                data.CorrectPassCGI = passCGIIT->second;
                            }else{
                                is_valid = false;
                                std::cout << "Pass CGI not found " << std::endl;
                                return;
                            }
    
    
                            cgiHandler->setVarsEnv(data);
                            
                            responses[client_fd] = cgiHandler->executeScript();
                            this->status = cgiHandler->getStatusCGI();
                            this->flagCGI = cgiHandler->getCGIFlag();
                            if (this->status != 200 && responses[client_fd].empty()){
                                is_valid = false;
                                is_Complet = true;
                            }else if (this->status == 504 && !responses[client_fd].empty())
                                is_Complet = true;
                            this->Cgi = true;
                    }
                    else if (!postHandler->isRequestComplete() && postHandler->getCGIState()){
                        std::cout << "is a CGI and parsRequest not Complete\n";
                    }
                    else if (postHandler->isRequestComplete() && !postHandler->getCGIState()){
                        std::cout << "is a not a CGI and parsRequest Complete\n";
                        is_Complet = true;
                    }
                }
                else{
                    std::cout << "content length negative\n";
                    is_valid = false;
                    header_parsed = false;
                    this->status = 400;
                    this->responses[client_fd] = statusMap[this->status];
                    return;
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
                if (postHandler->getStatus() != 200 )
                {
                    this->status = postHandler->getStatus();
                    std::cout << "ERROR 2" << std::endl;
                    is_valid = false;
                    is_Complet = true;
                    this->responses[client_fd] = statusMap[this->status];
                    return ;
                }
                if (postHandler->isRequestComplete() && postHandler->getCGIState()) {
                        std::cout << "is a CGI and parsRequest Complete\n";
                            cgiHandler =  new CGI();
                            dataCGI data;
                            data.method = method;
                            data.path = path;
                            data.version = version;
                            data.file = postHandler->getFilename();
                            data.contentType = postHandler->getContentType();
                            data.contentLen = postHandler->getCurrentLength();
                            data.scriptPath = postHandler->getScriptPath();
                            data.queryString = this->query;
                            if (postHandler->getAutoindexFromPost())
                                data.autoIndex = "true";
                            else
                                data.autoIndex = "false";
                            std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                            std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + postHandler->getExtension());
                            if (passCGIIT != passCGI.end()){
                                data.CorrectPassCGI = passCGIIT->second;
                            }else{
                                is_valid = false;
                                std::cout << "Pass CGI not found " << std::endl;
                                return;
                            }
    
    
                            cgiHandler->setVarsEnv(data);
                            
                            responses[client_fd] = cgiHandler->executeScript();
                            this->status = cgiHandler->getStatusCGI();
                            this->flagCGI = cgiHandler->getCGIFlag();
                            if (this->status != 200 && responses[client_fd].empty()){
                                is_valid = false;
                                is_Complet = true;
                            }else if (this->status == 504 && !responses[client_fd].empty())
                                is_Complet = true;
                            this->Cgi = true;
                }
                else if (!postHandler->isRequestComplete() && postHandler->getCGIState()){
                    std::cout << "is a CGI and parsRequest not Complete\n";
                }
                else if (postHandler->isRequestComplete() && !postHandler->getCGIState()){
                    std::cout << "is a not a CGI and parsRequest Complete\n";
                    is_Complet = true;
                }
    
            }else if (method == "POST" && !is_chunked && is_boundary){
    
                std::cout << "here i will hendl the boundary " << std::endl;
    
                std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
                if (contentTypeIt != headers.end()) {
                    contentType = contentTypeIt->second;
                }
                std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
                if (contentLengthIt != headers.end()) {
                    contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
                }
                std::string boundaryPrefix = "boundary=";
                size_t posBoundary = contentType.find(boundaryPrefix);
                if (posBoundary != std::string::npos)
                {
                    size_t startPos = posBoundary + boundaryPrefix.length();
                    boundaryValue = contentType.substr(startPos);
                }
                if (!postHandler)
                {
                    postHandler = new PostHandler();
                }
                postHandler->setSepa( "--" + boundaryValue + "\r\n");
                postHandler->setTer( "--" + boundaryValue + "--");
                postHandler->setExpextedLength(contentLength);
                postHandler->initBoundary(body, *this, parser);
                if (postHandler->getStatus() != 200)
                {
                    this->status = postHandler->getStatus();
                    std::cout << "ERROR 3 "  << status << std::endl;
                    this->responses[client_fd] = statusMap[this->status];
                    is_valid = false;
                    is_Complet = true;
                }
                if (postHandler->isRequestComplete())
                {
                    std::cout << "boundary is complete" << std::endl;
                    is_Complet = true;
                }
            }
            else if(method == "GET") 
            {
                std::cout << "*****GET requests " <<std::endl;
                if(!getHandler)
                    getHandler = new GetHandler();
                std::string response = getHandler->handleGetRequest(*this, parser);
                responses[client_fd] = response;
                if(getHandler->get_is_true_parse() == true)
                    is_Complet = true;
                if(getHandler->getCgiCheck() == true)
                    this->Cgi = true;
               
            }
            else if (method == "DELETE") 
            {
                std::cout << "*****DELETE requests " <<std::endl;
                DeleteHandler* deleteHandler = new DeleteHandler();
                std::string response = deleteHandler->handleDeleteRequest(*this, parser);
                responses[client_fd] = response;
                is_Complet = true;
                delete deleteHandler;
            }
        }

    }else if (method == "POST" && postHandler) {
        if (is_chunked) {
            if (postHandler->isRequestComplete())
            {
                if (postHandler->getCGIState() && this->cgiHandler){
                    responses[client_fd] = cgiHandler->executeScript();
                    this->status = cgiHandler->getStatusCGI();
                    this->flagCGI = cgiHandler->getCGIFlag();
                    
                    if (this->flagCGI == 5){
                        is_Complet = true;
                    }
                }

            }else
                postHandler->processChunkedData(request);
        }
        else if (is_boundary)
        {
            std::string body = request;
            postHandler->initBoundary(body, *this, parser);
            if (postHandler->getStatus() != 200){
                this->status = postHandler->getStatus();
                this->responses[client_fd] = statusMap[this->status];
                is_valid = false;
                is_Complet = true;
            }
        }
        else {
            if (postHandler->isRequestComplete())
            {
                if (postHandler->getCGIState() && this->cgiHandler){
                    responses[client_fd] = cgiHandler->executeScript();
                    this->status = cgiHandler->getStatusCGI();
                    this->flagCGI = cgiHandler->getCGIFlag();
                    
                    if (this->flagCGI == 5){
                        is_Complet = true;
                    }
                }

            }else{
                postHandler->processData(request);
            }
        }
        
        if (postHandler->isRequestComplete() && !postHandler->getCGIState()) {
            is_Complet = true;
        }
    }
    else if(method == "GET" && getHandler)
    {
        std::string response = getHandler->handleGetRequest(*this, parser);
        responses[client_fd] = response;
        is_Complet = getHandler->get_is_true_parse();
        this->Cgi = getHandler->getCgiCheck();
        if(is_Complet == true)
            std::cout << "is_Complet : true" << std::endl;
        else
            std::cout << "is_Complet : false" << std::endl;
    }
        
        


}
void ParsRequest::setResponses(std::string resp)
{
    responses[client_fd] = resp;
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

bool ParsRequest::getCGIState() const { return Cgi; }
bool ParsRequest::getFlagRedirect() const { return FlagRedirect; }
void ParsRequest::setFlagRedirect(){ FlagRedirect = true; }

const std::string& ParsRequest::getQuery() const { return query; }

int ParsRequest::getFlagCGI() const{ return flagCGI; }
bool ParsRequest::getFlagTimeOUT() const { return flagTimeOUT; }
bool ParsRequest::getFlagParsingHeader() const { return header_parsed; }

int ParsRequest::getStatus() const { return status; }