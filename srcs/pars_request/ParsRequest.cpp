#include "./ParsRequest.hpp"



ParsRequest::ParsRequest() {
    port = -1;
    header_parsed = false;
    is_valid = false;
    is_Complet = false;
    is_chunked = false;
    is_boundary = false;
    postHandler = NULL;
    getHandler = NULL;
    deleteHandler = NULL;
    FlagRedirect = false;
    flagTimeOUT = false;
    this->status = 200;
    errorFromConfig = false;
    errorReadComplete = false;
    contentType = "text/html";
    size = 0;
    totalBytesSent = 0;
    use_final_res = false;

    statusMessages[200] = "OK";
    statusMessages[400] = "Bad Request";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
    statusMessages[405] = "Method Not Allowed";
    statusMessages[408] = "Request Timeout";
    statusMessages[411] = "Length Required";
    statusMessages[413] = "Payload Too Large";
    statusMessages[415] = "Unsupported Media Type";
    statusMessages[414] = "URI Too Long";
    statusMessages[500] = "Internal Server Error";
    statusMessages[501] = "Not Implemented";
    statusMessages[504] = "Gateway Timeout";
    statusMessages[505] = "HTTP Version Not Supported";

 statusMap[200] = "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=utf-8\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "<!DOCTYPE html>"
                 "<html>"
                 "<head>"
                 "<meta charset=\"utf-8\">"
                 "<title>Success</title>"
                 "<style>"
                 "  body {"
                 "    font-family: 'Segoe UI', system-ui, sans-serif;"
                 "    background: #f8f9fa;"
                 "    display: flex;"
                 "    justify-content: center;"
                 "    align-items: center;"
                 "    height: 100vh;"
                 "    margin: 0;"
                 "    color: #333;"
                 "  }"
                 "  .success-card {"
                 "    background: white;"
                 "    border-radius: 12px;"
                 "    box-shadow: 0 4px 20px rgba(0,0,0,0.08);"
                 "    padding: 2rem;"
                 "    text-align: center;"
                 "    max-width: 400px;"
                 "    width: 90%;"
                 "    border-top: 4px solid #4BB543;"
                 "  }"
                 "  .success-icon {"
                 "    font-size: 3rem;"
                 "    color: #4BB543;"
                 "    margin-bottom: 1rem;"
                 "  }"
                 "  h1 {"
                 "    margin: 0 0 0.5rem;"
                 "    font-weight: 600;"
                 "    font-size: 1.5rem;"
                 "  }"
                 "  p {"
                 "    margin: 0;"
                 "    color: #666;"
                 "    line-height: 1.5;"
                 "  }"
                 "  .details {"
                 "    margin-top: 1.5rem;"
                 "    font-size: 0.8rem;"
                 "    color: #999;"
                 "  }"
                 "</style>"
                 "</head>"
                 "<body>"
                 "<div class=\"success-card\">"
                 "  <div class=\"success-icon\">&#10003;</div>"
                 "  <h1>Request Successful</h1>"
                 "  <p>Your request was processed successfully by the server.</p>"
                 "  <div class=\"details\">Status: 200 OK</div>"
                 "</div>"
                 "</body>"
                 "</html>";
    
    statusMap[400] = "HTTP/1.1 400 Bad Request\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Bad Request</title></head>"
                     "<body><h1>Invalid request syntax</h1></body></html>";

    statusMap[403] = "HTTP/1.1 403 Forbidden\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Forbidden</title></head>"
                     "<body><h1>Forbidden</h1></body></html>";
    
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
    statusMap[415] = "HTTP/1.1 415 Unsupported Media Type\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "<html><head><title>Unsupported Media Type</title></head>"
                     "<body><h1>Unsupported Media Type</h1></body></html>";
    
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

    contentTypes_1["html"] = "text/html";
    contentTypes_1["css"] = "text/css";
    contentTypes_1["xml"] = "text/xml";
    contentTypes_1["txt"] = "text/plain";
    contentTypes_1["md"] = "text/markdown";
    contentTypes_1["js"] = "text/javascript";
    contentTypes_1["json"] = "application/json";
    contentTypes_1["csv"] = "text/csv";
    

    contentTypes_1["gif"] = "image/gif";
    contentTypes_1["jpeg"] = "image/jpeg";
    contentTypes_1["jpg"] = "image/jpg";
    contentTypes_1["png"] = "image/png";
    contentTypes_1["webp"] = "image/webp";
    contentTypes_1["svg"] = "image/svg+xml";
    contentTypes_1["ico"] = "image/x-icon";
    contentTypes_1["bmp"] = "image/bmp";
    contentTypes_1["tiff"] = "image/tiff";
    

    contentTypes_1["mp3"] = "audio/mpeg";
    contentTypes_1["wav"] = "audio/wav";
    contentTypes_1["ogg"] = "audio/ogg";
    contentTypes_1["m4a"] = "audio/mp4";
    contentTypes_1["aac"] = "audio/aac";
    contentTypes_1["flac"] = "audio/flac";
    contentTypes_1["mid"] = "audio/midi";
    
   
    contentTypes_1["mp4"] = "video/mp4";
    contentTypes_1["webm"] = "video/webm";
    contentTypes_1["avi"] = "video/x-msvideo";
    contentTypes_1["mpg"] = "video/mpeg";
    contentTypes_1["mov"] = "video/quicktime";
    contentTypes_1["wmv"] = "video/x-ms-wmv";
    contentTypes_1["flv"] = "video/x-flv";
    
 
    contentTypes_1["pdf"] = "application/pdf";
    contentTypes_1["doc"] = "application/msword";
    contentTypes_1["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    contentTypes_1["xls"] = "application/vnd.ms-excel";
    contentTypes_1["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    contentTypes_1["ppt"] = "application/vnd.ms-powerpoint";
    contentTypes_1["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    contentTypes_1["zip"] = "application/zip";
    contentTypes_1["gz"] = "application/gzip";
    contentTypes_1["tar"] = "application/x-tar";
    contentTypes_1["rar"] = "application/x-rar-compressed";
    contentTypes_1["7z"] = "application/x-7z-compressed";
    contentTypes_1["exe"] = "application/x-msdownload";
    contentTypes_1["swf"] = "application/x-shockwave-flash";
    

    contentTypes_1["woff"] = "font/woff";
    contentTypes_1["woff2"] = "font/woff2";
    contentTypes_1["ttf"] = "font/ttf";
    contentTypes_1["otf"] = "font/otf";
    contentTypes_1["eot"] = "application/vnd.ms-fontobject";
    

    contentTypes_1["php"] = "text";
    contentTypes_1["py"] = "text/x-python";
    contentTypes_1["java"] = "text/x-java-source";
    contentTypes_1["c"] = "text/x-c";
    contentTypes_1["cpp"] = "text/x-c++";
    contentTypes_1["rb"] = "text/x-ruby";
    contentTypes_1["sh"] = "application/x-sh";
    contentTypes_1["pl"] = "text/x-perl";
    contentTypes_1["sql"] = "application/sql";
    contentTypes_1["xml"] = "application/xml";
    contentTypes_1["yaml"] = "text/yaml";
    contentTypes_1[""] = "application/octet-stream";
}

ParsRequest::~ParsRequest() {
    if (postHandler)
        delete postHandler;
    if(getHandler)
        delete getHandler;
    if(deleteHandler)
        delete deleteHandler;
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

void ParsRequest::parseRequestLine( std::string& line) {
    trim_crlf(line);
    std::vector<std::string> parts = split(line, ' ');
    if (parts.size() != 3) {
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return;
    }

    method = parts[0];
    path = normalizePath(parts[1]);
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
                return;
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
        is_valid = false;
        header_parsed = false;
        this->status = 400;
        return;
    }

}

std::string ParsRequest::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return "";
}

size_t ParsRequest::getFileSize(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string ParsRequest::readSmallFile(std::string filePath) {
    if(!file_error.is_open())
    {
        file_error.open(filePath.c_str(), std::ios::binary);
        if (!file_error) {
            return "";
        }
    }
    if(totalBytesSent >= size)
    {
        file_error.close();
        errorReadComplete = true;
        return "";
    }
    std::string content;
    char buffer[BUFFER_SIZE_P];
    memset(buffer, 0, BUFFER_SIZE_P);
    file_error.read(buffer, BUFFER_SIZE_P);
    content.append(buffer, file_error.gcount());
    final_res += content;
    totalBytesSent = totalBytesSent + file_error.gcount();
    return final_res;
}

std::string ParsRequest::normalizePath(const std::string& path) {
    std::vector<std::string> segments;
    std::istringstream iss(path);
    std::string segment;
    

    while (std::getline(iss, segment, '/')) {
        if (segment == "." || segment.empty()) {
            continue; 
        } else if (segment == "..") {
            if (!segments.empty()) {
                segments.pop_back();
            }
        } else {
            segments.push_back(segment);
        }
    }
    
    std::string result = "/";
    for (size_t i = 0; i < segments.size(); ++i) {
        if (i > 0) result += "/";
        result += segments[i];
    }
    return result;
}

void ParsRequest::parse(const std::string& request,int client_fd, ConfigParser &parser) {
    this->client_fd = client_fd;
    requestContent += request;
    std::string contentType = "";
    size_t contentLength = 0;
    std::string boundaryValue= "";
    
    if (errorFromConfig)
    {
        std::string content = readSmallFile(file_error_path);
        this->responses[client_fd] = content;
        final_res.clear();
        return ;
    }
    if (!header_parsed) {
        size_t header_end = requestContent.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            this->flagTimeOUT = true;
            return;
            
        } else{
            if (header_end == 0){
                return;
            }
            if (this->flagTimeOUT == true)
                this->flagTimeOUT = false;
            header_parsed = true;
            std::string header_section = requestContent.substr(0, header_end);
            this->body = requestContent.substr(header_end + 4);
            std::vector<std::string> lines = split(header_section, '\n');
            if (lines.empty()) {
                is_valid = false;
                return;
            }
            parseRequestLine(lines[0]);
            if (!is_valid){
                this->responses[client_fd] = statusMap[this->status];
                use_final_res = true;
            }
            else
            {
                parseHeaders(header_section);
                if (!is_valid){
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                }
            }

            server_it = parser.getServer_parser(this->host,this->port);
            if(server_it == parser.getServers().end())
            {
                if(status != 414)
                    status = 400;
                this->responses[client_fd] = statusMap[this->status];
                use_final_res = true;
                is_valid = false;
                header_parsed = false;
                return;

            }
            if(!is_valid)
            {
                std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                if(itse != server_it->getErrorPages().end())
                {
                    size = getFileSize(itse->second);
                    contentType = getFileExtension(itse->second);
                    std::stringstream  header;
                    header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                            << "Content-Length: " << size << "\r\n"
                    << "Content-Type: " << contentType << "\r\n"
                    << "Connection: close\r\n"
                    << "\r\n";
                    final_res += header.str();
                    file_error_path = itse->second;
                    errorFromConfig = true;
                    this->responses[client_fd] = final_res;
                    final_res.clear();
                    return ;
                }
                else
                    return ;
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
                        postHandler = new PostHandler();
                    postHandler->initialize(*this, parser);
                    if(FlagRedirect)
                    {
                        is_Complet = true;
                        return ;   
                    }
                    
                    if (postHandler->getStatus() != 200 )
                    {
                        this->status = postHandler->getStatus();
                        is_valid = false;
                        is_Complet = true;
                        std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                        if(itse != server_it->getErrorPages().end())
                        {
                            if (access(itse->second.c_str(), R_OK) == -1)
                            {
                                this->responses[client_fd] = statusMap[this->status];
                                use_final_res = true;
                                return ;
                            }
                            size = getFileSize(itse->second);
                            contentType = getFileExtension(itse->second);
                            std::stringstream  header;
                            header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                            << "Content-Type: " << contentType << "\r\n"
                            << "Content-Length: " << size << "\r\n"
                            << "Connection: close\r\n"
                            << "\r\n";
                            final_res += header.str();
                            file_error_path = itse->second;
                            errorFromConfig = true;
                            this->responses[client_fd] = final_res;
                            final_res.clear();
                            return ;
                        }
                        this->responses[client_fd] = statusMap[this->status];
                        use_final_res = true;
                        return ;
                    }
                    if (postHandler->isRequestComplete() && postHandler->getCGIState()) {
                            cgiHandler =  new CGI();
                            dataCGI data;
                            data.method = method;
                            data.path = path;
                            data.version = version;
                            data.file = postHandler->getFilename();
                            data.contentType = postHandler->getContentType();
                            data.contentLen = postHandler->getCurrentLength();
                            data.scriptPath = postHandler->getScriptPath();
                            data.headers = this->headers;
                            size_t pos = postHandler->getScriptPath().find_last_of(".");
                            std::string ext ;
                            data.queryString = this->query;
                            if (postHandler->getAutoindexFromPost())
                                data.autoIndex = "true";
                            else
                                data.autoIndex = "false";
                            if(pos != std::string::npos)
                            {
                                ext = postHandler->getScriptPath().substr(pos + 1, postHandler->getScriptPath().length());
                            }
                            else
                                ext = "";
                            
                            std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                            if (!ext.empty())
                            {
                                std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + ext);
                                if (passCGIIT != passCGI.end()){
                                    data.CorrectPassCGI = passCGIIT->second;
                                }else{

                                    is_valid = false;
                                    this->status = 404;
                                    this->responses[client_fd] = statusMap[this->status];
                                    use_final_res = true;
                                    return;
                                }
                            }
                            cgiHandler->setVarsEnv(data);
                            
                            responses[client_fd] = cgiHandler->executeScript();
                            this->status = cgiHandler->getStatusCGI();
                            this->flagCGI = cgiHandler->getCGIFlag();
                            if (this->status != 200 && responses[client_fd].empty()){
                                responses[client_fd] = statusMap[this->status];
                                is_valid = false;
                                is_Complet = true;
                            }else if (this->status == 504 && !responses[client_fd].empty())
                                is_Complet = true;
                            this->Cgi = true;
                            if (this->status != 200){
                                is_valid = false;
                                is_Complet = true;
                                std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                                if(itse != server_it->getErrorPages().end())
                                {
                                    if (access(itse->second.c_str(), R_OK) == -1)
                                    {
                                        this->responses[client_fd] = statusMap[this->status];
                                        use_final_res = true;
                                        return ;
                                    }
                                    size = getFileSize(itse->second);
                                    contentType = getFileExtension(itse->second);
                                    std::stringstream  header;
                                    header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                                    << "Content-Type: " << contentType << "\r\n"
                                    << "Content-Length: " << size << "\r\n"
                                    << "Connection: close\r\n"
                                    << "\r\n";
                                    final_res += header.str();
                                    file_error_path = itse->second;
                                    errorFromConfig = true;
                                    this->responses[client_fd] = final_res;
                                    final_res.clear();
                                    return ;
                                }
                                this->responses[client_fd] = statusMap[this->status];
                                use_final_res = true;
                                return;
                            }
                    }
                    else if (!postHandler->isRequestComplete() && postHandler->getCGIState()){
                            cgiHandler =  new CGI();
                            
                            dataCGI data;
                            data.method = method;
                            data.path = path;
                            data.version = version;
                            data.file = postHandler->getFilename();
                            data.contentType = postHandler->getContentType();
                            data.contentLen = postHandler->getCurrentLength();
                            data.scriptPath = postHandler->getScriptPath();
                            data.headers = this->headers;
                            size_t pos = postHandler->getScriptPath().find_last_of(".");
                            std::string ext ;
                            data.queryString = this->query;
                            if (postHandler->getAutoindexFromPost())
                                data.autoIndex = "true";
                            else
                                data.autoIndex = "false";
                            if(pos != std::string::npos)
                            {
                                ext = postHandler->getScriptPath().substr(pos + 1, postHandler->getScriptPath().length());
                            }
                            else 
                                ext = "";
                            
                            std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                            if (!ext.empty())
                            {
                                std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + ext);
                                if (passCGIIT != passCGI.end()){
                                    data.CorrectPassCGI = passCGIIT->second;
                                }else{

                                    is_valid = false;
                                    this->status = 404;
                                    this->responses[client_fd] = statusMap[this->status];
                                    use_final_res = true;
                                    return;
                                }
                            }
                            cgiHandler->setVarsEnv(data);
                    }
                    else if (postHandler->isRequestComplete() && !postHandler->getCGIState()){
                        is_Complet = true;
                        this->responses[client_fd] = statusMap[this->status];
                        use_final_res = true;
                    }
                }
                else{
                    is_valid = false;
                    header_parsed = false;
                    this->status = 400;
                    std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                    if(itse != server_it->getErrorPages().end())
                    {
                        if (access(itse->second.c_str(), R_OK) == -1)
                        {
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return ;
                        }
                        size = getFileSize(itse->second);
                        contentType = getFileExtension(itse->second);
                        std::stringstream  header;
                        header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                        << "Content-Type: " << contentType << "\r\n"
                        << "Content-Length: " << size << "\r\n"
                        << "Connection: close\r\n"
                        << "\r\n";
                        final_res += header.str();
                        file_error_path = itse->second;
                        errorFromConfig = true;
                        this->responses[client_fd] = final_res;
                        final_res.clear();
                        return ;
                    }
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                    return;
                }
            } 
            else if (method == "POST" && is_chunked && !is_boundary){
                std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
                if (contentTypeIt != headers.end()) {
                    contentType = contentTypeIt->second;
                }
                if (!postHandler)
                    postHandler = new PostHandler();
                postHandler->initialize(*this, parser);
                if (postHandler->getStatus() != 200 )
                {
                    this->status = postHandler->getStatus();
                    is_valid = false;
                    is_Complet = true;
                    std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                    if(itse != server_it->getErrorPages().end())
                    {
                        if (access(itse->second.c_str(), R_OK) == -1)
                        {
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return ;
                        }
                        size = getFileSize(itse->second);
                        contentType = getFileExtension(itse->second);
                        std::stringstream  header;
                        header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                        << "Content-Type: " << contentType << "\r\n"
                        << "Content-Length: " << size << "\r\n"
                        << "Connection: close\r\n"
                        << "\r\n";
                        final_res += header.str();
                        file_error_path = itse->second;
                        errorFromConfig = true;
                        this->responses[client_fd] = final_res;
                        final_res.clear();
                        return ;
                    }
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                    return ;
                }
                if (postHandler->isRequestComplete() && postHandler->getCGIState()) {
                    cgiHandler =  new CGI();
                    dataCGI data;
                    data.method = method;
                    data.path = path;
                    data.version = version;
                    data.file = postHandler->getFilename();
                    data.contentType = postHandler->getContentType();
                    data.contentLen = postHandler->getCurrentLength();
                    data.scriptPath = postHandler->getScriptPath();
                    data.headers = this->headers;
                    size_t pos = postHandler->getScriptPath().find_last_of(".");
                    std::string ext ;
                    data.queryString = this->query;
                    if (postHandler->getAutoindexFromPost())
                        data.autoIndex = "true";
                    else
                        data.autoIndex = "false";
                    if(pos != std::string::npos)
                        ext = postHandler->getScriptPath().substr(pos + 1, postHandler->getScriptPath().length());
                    else 
                        ext = "";
                    
                    std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                    if (!ext.empty())
                    {
                        std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + ext);
                        if (passCGIIT != passCGI.end()){
                            data.CorrectPassCGI = passCGIIT->second;
                        }else{

                            is_valid = false;
                            this->status = 404;
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return;
                        }
                    }
                    cgiHandler->setVarsEnv(data);
                    
                    responses[client_fd] = cgiHandler->executeScript();
                    this->status = cgiHandler->getStatusCGI();
                    this->flagCGI = cgiHandler->getCGIFlag();
                    if (this->status != 200 && responses[client_fd].empty()){
                        responses[client_fd] = statusMap[this->status];
                        is_valid = false;
                        is_Complet = true;
                    }else if (this->status == 504 && !responses[client_fd].empty())
                        is_Complet = true;
                    this->Cgi = true;
                    if (this->status != 200){
                        is_valid = false;
                        is_Complet = true;
                        std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                        if(itse != server_it->getErrorPages().end())
                        {
                            if (access(itse->second.c_str(), R_OK) == -1)
                            {
                                this->responses[client_fd] = statusMap[this->status];
                                use_final_res = true;
                                return ;
                            }
                            size = getFileSize(itse->second);
                            contentType = getFileExtension(itse->second);
                            std::stringstream  header;
                            header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                            << "Content-Type: " << contentType << "\r\n"
                            << "Connection: close\r\n"
                            << "\r\n";
                            final_res += header.str();
                            file_error_path = itse->second;
                            errorFromConfig = true;
                            this->responses[client_fd] = final_res;
                            final_res.clear();
                            return ;
                        }
                        this->responses[client_fd] = statusMap[this->status];
                        use_final_res = true;
                        return;
                    }
                }
                else if (!postHandler->isRequestComplete() && postHandler->getCGIState()){
                    cgiHandler =  new CGI();
                    
                    dataCGI data;
                    data.method = method;
                    data.path = path;
                    data.version = version;
                    data.file = postHandler->getFilename();
                    data.contentType = postHandler->getContentType();
                    data.contentLen = postHandler->getCurrentLength();
                    data.scriptPath = postHandler->getScriptPath();
                    data.headers = this->headers;
                    size_t pos = postHandler->getScriptPath().find_last_of(".");
                    std::string ext ;
                    data.queryString = this->query;
                    if (postHandler->getAutoindexFromPost())
                        data.autoIndex = "true";
                    else
                        data.autoIndex = "false";
                    if(pos != std::string::npos)
                        ext = postHandler->getScriptPath().substr(pos + 1, postHandler->getScriptPath().length());
                    else 
                        ext = "";
                    
                    std::map<std::string, std::string> passCGI = postHandler->getCgiPassFomPost();
                    if (!ext.empty())
                    {
                        std::cout << ext << std::endl;
                        std::cout << pos << std::endl;
                        std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + ext);
                        if (passCGIIT != passCGI.end()){
                            data.CorrectPassCGI = passCGIIT->second;
                        }else{

                            is_valid = false;
                            this->status = 404;
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return;
                        }
                    }
                    cgiHandler->setVarsEnv(data);
                }
                use_final_res = true;
                if (postHandler->isRequestComplete() && !postHandler->getCGIState()){
                    is_Complet = true;
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                }
                
    
            }else if (method == "POST" && !is_chunked && is_boundary){
                std::cout << "boundary\n";
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
                    is_valid = false;
                    is_Complet = true;
                    std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                    if(itse != server_it->getErrorPages().end())
                    {
                        if (access(itse->second.c_str(), R_OK) == -1)
                        {
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return ;
                        }
                        size = getFileSize(itse->second);
                        contentType = getFileExtension(itse->second);
                        std::stringstream  header;
                        header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                        << "Content-Type: " << contentType << "\r\n"
                        << "Content-Length: " << size << "\r\n"
                        << "Connection: close\r\n"
                        << "\r\n";
                        final_res += header.str();
                        file_error_path = itse->second;
                        errorFromConfig = true;
                        this->responses[client_fd] = final_res;
                        final_res.clear();
                        return ;
                    }
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                    return;
                }
                if (postHandler->isRequestComplete())
                {
                    is_Complet = true;
                    this->responses[client_fd] = statusMap[this->status];
                    use_final_res = true;
                }
            }
            else if(method == "GET") 
            {
                if(!getHandler)
                    getHandler = new GetHandler();
                std::string response = getHandler->handleGetRequest(*this, parser);
                responses[client_fd] = response;
                if(getHandler->get_is_true_parse() == true)
                    is_Complet = true;
                if(getHandler->getCgiCheck() == true)
                    this->Cgi = true;
                use_final_res = getHandler->get_use_final_res();
               
            }
            else if (method == "DELETE") 
            {
                if(!deleteHandler)
                    deleteHandler = new DeleteHandler();
                std::string response = deleteHandler->handleDeleteRequest(*this, parser);
                responses[client_fd] = response;
                if(deleteHandler->get_is_true_parse() == true)
                    is_Complet = true;
                use_final_res = deleteHandler->get_use_final_res();
            }
        }

    }else if (method == "POST" && postHandler) {
        if (is_chunked) {
           
            if (!postHandler->isRequestComplete())
            {
                postHandler->processChunkedData(request);
            }
            if (postHandler->isRequestComplete()){
                if (postHandler->getCGIState() && this->cgiHandler){
                    this->Cgi = true;
                    responses[client_fd] = cgiHandler->executeScript();
                    this->status = cgiHandler->getStatusCGI();
                    this->flagCGI = cgiHandler->getCGIFlag();
                    if (this->flagCGI == 5){
                        if(responses[client_fd].empty()){
                            responses[client_fd] = statusMap[this->status];
                        }
                        if (this->status != 200){
                        
                            is_valid = false;
                            is_Complet = true;
                            std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                            if(itse != server_it->getErrorPages().end())
                            {
                                if (access(itse->second.c_str(), R_OK) == -1)
                                {
                                    this->responses[client_fd] = statusMap[this->status];
                                    use_final_res = true;
                                    return ;
                                }
                                size = getFileSize(itse->second);
                                contentType = getFileExtension(itse->second);
                                std::stringstream  header;
                                header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                                << "Content-Type: " << contentType << "\r\n"
                                << "Content-Length: " << size << "\r\n"
                                << "Connection: close\r\n"
                                << "\r\n";
                                final_res += header.str();
                                file_error_path = itse->second;
                                errorFromConfig = true;
                                this->responses[client_fd] = final_res;
                                final_res.clear();
                                return ;
                            }
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                            return;
                        }
                        is_Complet = true;
                    }
                    
                }
            }

        }
        else if (is_boundary)
        {
            std::string body = request;
            postHandler->initBoundary(body, *this, parser);
            if (postHandler->getStatus() != 200){
                this->status = postHandler->getStatus();
                is_valid = false;
                is_Complet = true;
                std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                if(itse != server_it->getErrorPages().end())
                {
                    if (access(itse->second.c_str(), R_OK) == -1)
                    {
                        this->status = 403;
                        std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                        if(itse != server_it->getErrorPages().end()){
                            if (access(itse->second.c_str(), R_OK) == -1)
                            {
                                this->responses[client_fd] = statusMap[this->status];
                                use_final_res = true;
                                return ;
                            }
                            size = getFileSize(itse->second);
                            contentType = getFileExtension(itse->second);
                            std::stringstream  header;
                            header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                            << "Content-Type: " << contentType << "\r\n"
                            << "Content-Length: " << size << "\r\n"
                            << "Connection: close\r\n"
                            << "\r\n";
                            final_res += header.str();
                            file_error_path = itse->second;
                            errorFromConfig = true;
                            this->responses[client_fd] = final_res;
                            final_res.clear();
                            return ;
                        }
                        this->responses[client_fd] = statusMap[this->status];
                        use_final_res = true;
                        return ;
                    }
                    size = getFileSize(itse->second);
                    contentType = getFileExtension(itse->second);
                    std::stringstream  header;
                    header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                    << "Content-Type: " << contentType << "\r\n"
                    << "Content-Length: " << size << "\r\n"
                    << "Connection: close\r\n"
                    << "\r\n";
                    final_res += header.str();
                    file_error_path = itse->second;
                    errorFromConfig = true;
                    this->responses[client_fd] = final_res;
                    final_res.clear();
                    return ;
                }
                this->responses[client_fd] = statusMap[this->status];
                use_final_res = true;
                return;
            }
        }
        else {
            if (!postHandler->isRequestComplete()){
                postHandler->processData(request);
            }
            if (postHandler->isRequestComplete())
            {
                if (postHandler->getCGIState() && this->cgiHandler){
                    this->Cgi = true;
                    responses[client_fd] = cgiHandler->executeScript();
                    this->status = cgiHandler->getStatusCGI();
                    this->flagCGI = cgiHandler->getCGIFlag();
                    if (this->flagCGI == 5){
                        if(responses[client_fd].empty()){
                            responses[client_fd] = statusMap[this->status];
                        }
                        if (this->status != 200){
                        
                            is_valid = false;
                            is_Complet = true;
                            std::map<int, std::string>::const_iterator itse = server_it->getErrorPages().find(status);
                            if(itse != server_it->getErrorPages().end())
                            {
                                if (access(itse->second.c_str(), R_OK) == -1)
                                {
                                    this->responses[client_fd] = statusMap[this->status];
                                    use_final_res = true;
                                    return ;
                                }
                                size = getFileSize(itse->second);
                                contentType = getFileExtension(itse->second);
                                std::stringstream  header;
                                header  << "HTTP/1.1 "<< status << " " <<  statusMessages[status] << "\r\n"
                                << "Content-Type: " << contentType << "\r\n"
                                << "Content-Length: " << size << "\r\n"
                                << "Connection: close\r\n"
                                << "\r\n";
                                final_res += header.str();
                                file_error_path = itse->second;
                                errorFromConfig = true;
                                this->responses[client_fd] = final_res;
                                final_res.clear();
                                return ;
                            }
                            this->responses[client_fd] = statusMap[this->status];
                            use_final_res = true;
                        return;
                    }
                        is_Complet = true;
                    }
                    
                }

            }
            
        }
        if (postHandler->isRequestComplete() && !postHandler->getCGIState()) {
            is_Complet = true;
            this->responses[client_fd] = statusMap[this->status];
            use_final_res = true;
        }
        
    }
    else if(method == "GET" && getHandler)
    {
        std::string response = getHandler->handleGetRequest(*this, parser);
        responses[client_fd] = response;
        is_Complet = getHandler->get_is_true_parse();
        this->Cgi = getHandler->getCgiCheck();

        use_final_res = getHandler->get_use_final_res();
    }
    else if(method == "DELETE" && deleteHandler)
    {
        std::string response = deleteHandler->handleDeleteRequest(*this, parser);
        responses[client_fd] = response;
        if(deleteHandler->get_is_true_parse() == true)
            is_Complet = true;
    }
            


}
void ParsRequest::setResponses(std::string resp){responses[client_fd] = resp;}
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

const int& ParsRequest::getClientFd() const{return client_fd;}
const std::map<int,std::string>& ParsRequest::getResponses() const { return responses; }

bool ParsRequest::getCGIState() const { return Cgi; }
bool ParsRequest::getFlagRedirect() const { return FlagRedirect; }
void ParsRequest::setFlagRedirect(){ FlagRedirect = true; }

const std::string& ParsRequest::getQuery() const { return query; }

int ParsRequest::getFlagCGI() const{ return flagCGI; }
bool ParsRequest::getFlagTimeOUT() const { return flagTimeOUT; }


bool ParsRequest::getErrorFromConfig() const { return errorFromConfig; }
void ParsRequest::setErrorFromConfig(){ errorFromConfig = false; }
bool ParsRequest::getErrorReadComplete() const { return errorReadComplete; }
void ParsRequest::setErrorReadComplete(){ errorReadComplete = false; }

bool ParsRequest::getFlagParsingHeader() const { return header_parsed; }

int ParsRequest::getStatus() const { return status; }
bool ParsRequest::get_use_final_res() const{return use_final_res;}
void ParsRequest::set_use_final_res() { use_final_res = true;}