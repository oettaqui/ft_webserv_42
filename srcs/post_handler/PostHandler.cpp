#include "PostHandler.hpp"

PostHandler::PostHandler() : bodyLength(0), expectedLength(0), isComplete(false){
    storeContentTypes();
    this->status = 200;
    this->filename= "";

    this->extension = "";
    headers = "";
    content = "";

    leftoverData = "";
    check = 0;

    this->sep = ""; 
    this->terminator = ""; 

    this->isCGI = false;
    this->scriptPath = "";

    this->autoIndex = false;

    bufferOffset = 0;
}

PostHandler::~PostHandler() {
    if (file.is_open()) {
        file.close();
    }
}

void PostHandler::storeContentTypes() {

    contentTypes["text/html"] = "html";
    contentTypes["text/css"] = "css";
    contentTypes["text/xml"] = "xml";
    contentTypes["text/plain"] = "txt";
    contentTypes["text/markdown"] = "md";
    contentTypes["text/javascript"] = "js";
    contentTypes["application/json"] = "json";
    contentTypes["text/csv"] = "csv";
    

    contentTypes["image/gif"] = "gif";
    contentTypes["image/jpeg"] = "jpeg";
    contentTypes["image/jpg"] = "jpg";
    contentTypes["image/png"] = "png";
    contentTypes["image/webp"] = "webp";
    contentTypes["image/svg+xml"] = "svg";
    contentTypes["image/x-icon"] = "ico";
    contentTypes["image/bmp"] = "bmp";
    contentTypes["image/tiff"] = "tiff";
    

    contentTypes["audio/mpeg"] = "mp3";
    contentTypes["audio/wav"] = "wav";
    contentTypes["audio/ogg"] = "ogg";
    contentTypes["audio/mp4"] = "m4a";
    contentTypes["audio/aac"] = "aac";
    contentTypes["audio/flac"] = "flac";
    contentTypes["audio/midi"] = "mid";
    
    contentTypes["video/mp4"] = "mp4";
    contentTypes["video/webm"] = "webm";
    contentTypes["video/x-msvideo"] = "avi";
    contentTypes["video/mpeg"] = "mpg";
    contentTypes["video/quicktime"] = "mov";
    contentTypes["video/x-ms-wmv"] = "wmv";
    contentTypes["video/x-flv"] = "flv";

    contentTypes["application/pdf"] = "pdf";
    contentTypes["application/msword"] = "doc";
    contentTypes["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = "docx";
    contentTypes["application/vnd.ms-excel"] = "xls";
    contentTypes["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = "xlsx";
    contentTypes["application/vnd.ms-powerpoint"] = "ppt";
    contentTypes["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = "pptx";
    contentTypes["application/zip"] = "zip";
    contentTypes["application/gzip"] = "gz";
    contentTypes["application/x-tar"] = "tar";
    contentTypes["application/x-rar-compressed"] = "rar";
    contentTypes["application/x-7z-compressed"] = "7z";
    contentTypes["application/x-msdownload"] = "exe";
    contentTypes["application/x-shockwave-flash"] = "swf";
    

    contentTypes["font/woff"] = "woff";
    contentTypes["font/woff2"] = "woff2";
    contentTypes["font/ttf"] = "ttf";
    contentTypes["font/otf"] = "otf";
    contentTypes["application/vnd.ms-fontobject"] = "eot";
    

    contentTypes["application/x-httpd-php"] = "php";
    contentTypes["text/x-python"] = "py";
    contentTypes["text/x-java-source"] = "java";
    contentTypes["text/x-c"] = "c";
    contentTypes["text/x-c++"] = "cpp";
    contentTypes["text/x-ruby"] = "rb";
    contentTypes["application/x-sh"] = "sh";
    contentTypes["text/x-perl"] = "pl";
    contentTypes["application/sql"] = "sql";
    contentTypes["application/xml"] = "xml";
    contentTypes["text/yaml"] = "yaml";
    contentTypes["application/octet-stream"] = "";
}

bool PostHandler::fileExists(const std::string& path) {
    std::ifstream file(path.c_str());
    return file.good();
}

bool PostHandler::directoryExists(const std::string& path) {
    struct stat info;
    
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

std::string PostHandler::createUniqueFile(const std::string& extension, std::string& location_path) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::ostringstream filename;

    
    if (directoryExists(location_path))
    {
        if (!location_path.empty() && location_path[location_path.length() - 1] != '/' ) {
            location_path += '/';
            filename  << location_path.c_str() << "file_" << tv.tv_sec << "_" << tv.tv_usec;
        }
        else if (!location_path.empty() && location_path == "/")
        {
            filename  << "file_" << tv.tv_sec << "_" << tv.tv_usec;
        }
        else if (!location_path.empty())
        {
            filename  << location_path.c_str() << "file_" << tv.tv_sec << "_" << tv.tv_usec;
        }
        
    }
     else{
                   
            if (!fileExists(location_path)) {
                status = 404;
                return "";
            } else {
                status = 403;
                return "";
            }
        
        status = 404;
        return "";
    }
    
    
    if (!extension.empty()) {
        filename << "." << extension;
    }
    

    std::ofstream outfile(filename.str().c_str(), std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file: " << filename.str() << std::endl;
        status = 500;
        return "";
    }
    outfile.close();
    return filename.str();
}


bool PostHandler::fileExistsAndNotEmpty(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file && file.peek() != std::ifstream::traits_type::eof();
}

std::string PostHandler::getTheValidIndex(std::vector<std::string> index, std::string path) {


    if (path.empty() || path[path.length() - 1] != '/') {
        path += "/";
    }

    std::string tmp;
    std::vector<std::string>::iterator it = index.begin();
    while (it != index.end()) {
        tmp = path + *it;
        if (fileExistsAndNotEmpty(tmp)) {
            return tmp; 
        }
        ++it;
    }

    return "";
}

void PostHandler::initialize(ParsRequest &data_req, ConfigParser &parser) {

    std::string contentType = "";
    size_t contentLength = 0;
    std::map<std::string, std::string> headers = data_req.getHeaders();
    this->isChunked = data_req.isChunked();
   
    std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
    if (contentTypeIt != headers.end()) {
        contentType = contentTypeIt->second;
        cType = contentType;
    }
    if (!data_req.isChunked())
    {
        std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
        if (contentLengthIt != headers.end()) {
            contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
        }
    }
    this->expectedLength = contentLength;
    this->bodyLength = 0;
    this->isComplete = false;
    this->body = "";
    std::string correctPath = "";
    std::pair<std::string, Location> LocationAndPath;

    Server server = parser.getServer(data_req.hostMethod(), data_req.portMethod());
    
    locations = server.getLocations();

    LocationAndPath = getCorrectPath(locations, data_req.getPath());
    correctPath = LocationAndPath.first;
    size_t Ppos = correctPath.find(".");
    if ( Ppos != std::string::npos && Ppos + 1 < correctPath.length() && correctPath[Ppos + 1] != '/' && Ppos == 0)
    {
        correctPath = correctPath.substr(0, Ppos + 1) + "/" + correctPath.substr(Ppos + 1, correctPath.length());
    }
    if (correctPath.empty()){
        this->status = 404;
        return;
    }
    std::string location_path = "";
    Location location ;
    std::string p = correctPath;

    size_t fp;
    std::string fileN = "";
    std::string l;
    std::vector<std::string> indexV;
    
    int c = 0;
    if (p.find(".php") != std::string::npos || p.find(".py") != std::string::npos || p.find(".pl") != std::string::npos)
    {
        this->scriptPath = p;
        c = 1;
        fp = p.find_last_of("/", p.length());
        l = p.substr(0, fp);
        fileN = p.substr(fp + 1, p.length());
    }
    else if (p.find_last_of(".", p.length()) != std::string::npos)
        c = 2;
    location = LocationAndPath.second;
    std::string upload_store_path = location.getUploadStore();
    if (!upload_store_path.empty() && !location.getCgi())
        correctPath = upload_store_path;
    std::vector<std::string> allow_methods = location.getMethods();
    if(std::find(allow_methods.begin(), allow_methods.end(), "POST") == allow_methods.end())
    {
        status = 405;
        return;
    }
    if(location.hasRedirect() == true)
    {
        const std::map<int, std::string>::const_iterator redirection = location.getRedirection().begin();
        std::string content = createRedirectResponse(redirection->first,redirection->second);
        data_req.setResponses(content);
        data_req.setFlagRedirect();
        return ;
    }
    if (location.getCgi())
    {
        this->isCGI = true;
 
        this->cgiPassMap = location.getCgiPass();
        
        this->cgi_pass = location.getCgiPass();
        this->autoIndex = location.getAutoindex();
        if (c != 1)
            location_path = correctPath;
        else
            location_path = l;
 


        if (c != 1){
            if (this->autoIndex)
            {
                indexV = location.getIndex();
                
                std::string tmp = getTheValidIndex(indexV, location_path);
                if (!tmp.empty() && (tmp.find(".php") != std::string::npos || tmp.find(".py") != std::string::npos || tmp.find(".pl") != std::string::npos))
                {
                    location_path = tmp;
                    this->scriptPath = location_path;
                    c = 1;
                    fp = location_path.find_last_of("/", p.length());
                    l = location_path.substr(0, fp);
                    fileN = location_path.substr(fp + 1, location_path.length());
                }
                else if (tmp.empty() || (tmp.find(".php") == std::string::npos && tmp.find(".py") == std::string::npos && tmp.find(".pl") == std::string::npos)){
                    this->status = 403;
                    return;
                }
            }
            else{
                this->isCGI = false;
                this->status = 403;
                return;
            }

        }
    }else{
        this->isCGI = false;
        c = 2;
    }
    if (c != 1)
    {
        location_path = correctPath;
    }
    else{
        location_path = l;
    }

    this->maxBodySize = server.getClientMaxBodySize();
    
    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
    if (itT != contentTypes.end()) {
        this->extension = itT->second;
       
    } else {
        if (contentType == "application/x-www-form-urlencoded")
        {
            if (fileN.empty())
                this->extension = "txt";
            else{
                size_t pointP = fileN.find(".");
                if (pointP != std::string::npos)
                    this->extension = fileN.substr(pointP + 1, fileN.length());
            }
        }
        else
            status = 415;
           
    }
    if (expectedLength > maxBodySize && maxBodySize > 0 && expectedLength > 0){
        isComplete = true;
        this->status = 413;
        return;
    }else{
        this->filename = createUniqueFile(extension, location_path);
        if (status != 200)
            return;
        if (filename.empty()) {
            status = 500;
            return;
        }
    }
    
    file.open(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to Open File (500)" << std::endl;
        status = 500;
        return;
    }
    if (isChunked) {
        this->expectedLength = 0;
        this->chunkState = READING_SIZE;
        this->currentChunkSize = -1;
        this->currentChunkBytesRead = 0;
        this->chunkSizeBuffer = "";
    }
    
    if (!data_req.getBody().empty()) {
        if (isChunked) {
            processChunkedData(data_req.getBody());
        } else {
            processData(data_req.getBody());
        }
    }
    status = 200;
}



std::string PostHandler::extractContentType(const std::string& headers) {
    std::string contentTypePrefix = "Content-Type: ";
    size_t contentTypePos = headers.find(contentTypePrefix);
    
    if (contentTypePos != std::string::npos) {
        contentTypePos += contentTypePrefix.length();

        size_t lineEnd = headers.find("\r\n", contentTypePos);
        if (lineEnd == std::string::npos) {
            lineEnd = headers.length();
        }
        
        return headers.substr(contentTypePos, lineEnd - contentTypePos);
    }

    return "text/plain";
}


void PostHandler::processBoundaryData(std::string& initBody, ParsRequest &data_req, std::string& location_path){
    
    initBody = this->leftoverData + initBody;
    std::string body = initBody;
    size_t tPos = 0;
    size_t sPos = 0;
    size_t hPos = 0;
    this->leftoverData = "";

    tPos = body.find(terminator);
    if (tPos != std::string::npos){

        if (tPos >= 2 && body.substr(tPos-2, 2) == "\r\n") {
            std::string before_terminator = body.substr(0, tPos-2);
            this->check = 1;
            processBoundaryData(before_terminator, data_req, location_path);
        } else {
            std::string before_terminator = body.substr(0, tPos);
            this->check = 1;
            processBoundaryData(before_terminator, data_req, location_path);
        }
    }else{
        if ((sPos = body.find(sep)) != std::string::npos){
            if (sPos > 0)
            {
                this->content = body.substr(0, sPos);
                if (this->file.is_open())
                {
                    this->file << this->content;
                    this->content = "";
                    this->file.flush();
                    this->file.close();
                }
                body = body.substr(sPos, body.length());
                sPos = 0;

            }
            if (sPos == 0) {
                body = body.substr(sep.length(), body.length());
                if ((hPos = body.find("\r\n\r\n")) != std::string::npos){
                    this->headers = body.substr(0 , hPos);
                    std::string contentType = extractContentType(this->headers);
                    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
                    if (itT != contentTypes.end()) {
                        extension = itT->second;
                    } 
                    if (this->file.is_open())
                    {
                        this->file.flush();
                        this->file.close();
                    }
                    this->filename = createUniqueFile(extension, location_path);
                    if (this->status == 404)
                        return;
                    if (!filename.empty()) {
                        this->status = 200;
                    }
                    else{
                        this->status = 500;
                        return;
                    }
                    file.open(filename.c_str(), std::ios::binary);
                    if (!file) {
                        std::cerr << "Failed to open file for writing: " << filename << std::endl;
                        this->status = 500;
                        return;
                    }
                    body = body.substr(hPos + 4, body.length());
                }
                
            }
            processBoundaryData(body, data_req, location_path);
        }else if ((sPos = body.find(sep)) == std::string::npos){
            if (check == 0){
                if (body.length() > sep.length())
                {
                    this->leftoverData = body.substr(body.length() - sep.length(), body.length());
                    this->file << body.substr(0, body.length() - sep.length());
                    
                }else{
                    this->leftoverData = body;
                }
            }else{
                this->file << body;
            }
        }
        if (this->check == 1)
        {
            this->file.flush();
            this->file.close();
            isComplete = true;
            return;
        }
    }
}




void PostHandler::initBoundary(const std::string& initBody, ParsRequest &data_req, ConfigParser &parser) {

    this->bodyLength = 0;
    this->isComplete = false;
    

    Server server = parser.getServer(data_req.hostMethod(), data_req.portMethod());
    locations = server.getLocations();
    
    std::string correctPath = "";
    std::pair<std::string, Location> LocationAndPath;

    LocationAndPath = getCorrectPath(locations, data_req.getPath());
    correctPath = LocationAndPath.first;
    
    if (correctPath.empty()){
        this->status = 404;
        return;
    }
    std::string location_path = "";
    Location location ;
    location = LocationAndPath.second;
    std::vector<std::string> allow_methods = location.getMethods();
    std::string upload_store_path = location.getUploadStore();
    if (!upload_store_path.empty())
        correctPath = upload_store_path;
    location_path = correctPath;
    if (std::find(allow_methods.begin(), allow_methods.end(), "POST") != allow_methods.end()) {
    
        std::string body = initBody; 
        processBoundaryData(body, data_req, location_path);
    } else {
        status = 405;
        return;
    }
    if (location.getCgi())
    {
        if (data_req.isBoundary())
        {
            this->status = 400;
            return;
        }
        this->isCGI = true;
        this->cgi_pass = location.getCgiPass();
        this->autoIndex = location.getAutoindex();
        status = 400;
        return;

    }else{
        this->isCGI = false;
    }
    
    
}


void PostHandler::processData(const std::string& data) {

    if (!file.is_open()) {
        status = 500;
        std::cerr << "Error: File is not open when trying to process data" << std::endl;
        return;
    }
    
    file << data;
    

    bodyLength += data.length();

    if (bodyLength >= expectedLength) {
        file.flush();
        file.close();
        isComplete = true;
        std::cout << "File upload complete: " << filename << " (" << bodyLength << " bytes)" << std::endl;
        status = 200;
    }
}


void PostHandler::processChunkedData(const std::string& data) {

    buffer.append(data);
    processBufferedData();
}


void PostHandler::processBufferedData() {
    size_t pos = bufferOffset;
    
    while (pos < buffer.length()) {
        switch (chunkState) {
            case READING_SIZE: {
                size_t endPos = buffer.find("\r\n", pos);
                if (endPos == std::string::npos) {
                    chunkSizeBuffer = buffer.substr(pos); 
                    bufferOffset = pos;
                    return;
                }
                
                chunkSizeBuffer = buffer.substr(pos, endPos - pos);
                pos = endPos + 2;
                
                std::istringstream ss(chunkSizeBuffer);
                ss >> std::hex >> currentChunkSize;
                
                if (currentChunkSize == 0) {
                    chunkState = END_OF_CHUNKS;
                    isComplete = true;
                    file.flush();
                    file.close();
                    std::cout << "Chunked upload complete: " << filename 
                              << " (" << bodyLength << " bytes)" << std::endl;
                    buffer.clear();
                    bufferOffset = 0;
                    return;
                }
                
                currentChunkBytesRead = 0;
                chunkState = READING_DATA;
                break;
            }
            
            case READING_DATA: {
                size_t bytesRemaining = currentChunkSize - currentChunkBytesRead;
                size_t bytesAvailable = buffer.length() - pos;
                
                if (bytesRemaining == 0) {
                    chunkState = READING_TRAILING_CRLF;
                    break;
                }
                
                if (bytesAvailable == 0) {
                    bufferOffset = pos;
                    return;
                }
                
                size_t bytesToRead = std::min(bytesAvailable, bytesRemaining);
                
                if (bytesToRead > 0) {
                    file.write(buffer.c_str() + pos, bytesToRead);
                    bodyLength += bytesToRead;
                    currentChunkBytesRead += bytesToRead;
                    pos += bytesToRead;
                }
                
                if (currentChunkBytesRead >= currentChunkSize) {
                    chunkState = READING_TRAILING_CRLF;
                }
                break;
            }
            
            case READING_TRAILING_CRLF: {
                if (buffer.length() - pos < 2) {
                    bufferOffset = pos;
                    return;
                }
                
                
                if (buffer[pos] != '\r' || buffer[pos+1] != '\n') {
                    
                    chunkState = END_OF_CHUNKS;
                    isComplete = true;
                    file.close();
                    std::cerr << "Invalid chunk trailing CRLF" << std::endl;
                    status = 400;
                    buffer.clear();
                    bufferOffset = 0;
                    return;
                }
                
                pos += 2;
                chunkState = READING_SIZE;
                break;
            }
            
            case END_OF_CHUNKS:
                buffer.clear();
                bufferOffset = 0;
                return;
        }
        
        
        if (pos > 1024) {  
            buffer.erase(0, pos);
            pos = 0;
            bufferOffset = 0;
        }
    }

    buffer.clear();
    bufferOffset = 0;
}



bool PostHandler::isRequestComplete() const {
    return isComplete;
}

const std::string& PostHandler::getBody() const {
    return body;
}

const std::string& PostHandler::getFilename() const {
    return filename;
}

size_t PostHandler::getCurrentLength() const {
    return bodyLength;
}

void PostHandler::setExpextedLength(size_t len) {
    expectedLength = len;
}
int PostHandler::getStatus() const {
    return status;
}
void PostHandler::setSepa(std::string sep){
    this->sep = sep;
}

void PostHandler::setTer(std::string ter){
    this->terminator = ter;
}


bool PostHandler::getCGIState() const{
    return isCGI;
}

std::string PostHandler::getContentType () const{
    return cType;
}

std::string PostHandler::getScriptPath () const{
    return scriptPath;
}

std::map<std::string, std::string> PostHandler::getCgiPass() const 
{ 
    return cgiPassMap;
}


std::pair<std::string, Location> PostHandler::getCorrectPath(const std::map<std::string, Location>& locations, std::string path){

    std::string tmp = url_decode(path);
    std::string notLocation;
    std::string rest = "";
    
    while (!tmp.empty()){
        std::map<std::string, Location>::const_iterator it = locations.find(tmp);
        if (it != locations.end()) {
            std::string root = it->second.getRoot();
            if (!root.empty() && root[root.size() - 1] != '/' && !rest.empty() && rest[0] != '/')
            {
                root += "/";
            }
            std::string result = root + rest;
            
            return std::make_pair(result, it->second);
        }

        size_t lastSlash = tmp.find_last_of('/');
        if (lastSlash == std::string::npos || lastSlash == 0) {
            if (lastSlash == 0 && tmp.length() > 0){
                rest = tmp.substr(1, tmp.length()) + rest;
                tmp = "/";
            }
            else
                tmp = "/";
            
        } else {
            rest = tmp.substr(lastSlash, tmp.length()) + rest;
            tmp = tmp.substr(0, lastSlash);
        }
    }

    status = 404;
    return std::make_pair("", Location());
}


bool PostHandler::getAutoindexFromPost() const 
{ 
    return this->autoIndex;
}

const std::map<std::string, std::string>& PostHandler::getCgiPassFomPost() const 
{ 
    return this->cgiPassMap;
}

const std::string& PostHandler::getExtension() const{
    return this->extension;
}

std::string PostHandler::url_decode(std::string url) {
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%') {
            int hex = strtol(url.substr(i+1, 2).c_str(), 0, 16);
            url.replace(i, 3, 1, char(hex));
        }
    }
    return url;
}




std::string PostHandler::createRedirectResponse(int statusCode, const std::string& location) {
    std::stringstream response;
    
    response << "HTTP/1.1 " << statusCode << " ";
    

    switch(statusCode) {
        case 301: response << "Moved Permanently"; break;
        case 302: response << "Found"; break;
        case 303: response << "See Other"; break;
        case 307: response << "Temporary Redirect"; break;
        case 308: response << "Permanent Redirect"; break;
        default: response << "Redirect"; break;
    }
    
    response << "\r\n";
    response << "Location: " << location << "\r\n";
    response << "Content-Length: 0\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    
    return response.str();
}