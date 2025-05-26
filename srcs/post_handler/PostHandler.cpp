#include "PostHandler.hpp"

PostHandler::PostHandler() : bodyLength(0), expectedLength(0), isComplete(false){
    storeContentTypes();
    this->status = 0;
    this->filename= "";

    extension = "";
    headers = "";
    content = "";

    leftoverData = "";
    check = 0;

    this->sep = ""; 
    this->terminator = ""; 
}

PostHandler::~PostHandler() {
    if (file.is_open()) {
        file.close();
    }
}

void PostHandler::storeContentTypes() {
    // Text formats
    contentTypes["text/html"] = "html";
    contentTypes["text/css"] = "css";
    contentTypes["text/xml"] = "xml";
    contentTypes["text/plain"] = "txt";
    contentTypes["text/markdown"] = "md";
    contentTypes["text/javascript"] = "js";
    contentTypes["application/json"] = "json";
    contentTypes["text/csv"] = "csv";
    
    // Image formats
    contentTypes["image/gif"] = "gif";
    contentTypes["image/jpeg"] = "jpeg";
    contentTypes["image/jpg"] = "jpg";
    contentTypes["image/png"] = "png";
    contentTypes["image/webp"] = "webp";
    contentTypes["image/svg+xml"] = "svg";
    contentTypes["image/x-icon"] = "ico";
    contentTypes["image/bmp"] = "bmp";
    contentTypes["image/tiff"] = "tiff";
    
    // Audio formats
    contentTypes["audio/mpeg"] = "mp3";
    contentTypes["audio/wav"] = "wav";
    contentTypes["audio/ogg"] = "ogg";
    contentTypes["audio/mp4"] = "m4a";
    contentTypes["audio/aac"] = "aac";
    contentTypes["audio/flac"] = "flac";
    contentTypes["audio/midi"] = "mid";
    
    // Video formats
    contentTypes["video/mp4"] = "mp4";
    contentTypes["video/webm"] = "webm";
    contentTypes["video/x-msvideo"] = "avi";
    contentTypes["video/mpeg"] = "mpg";
    contentTypes["video/quicktime"] = "mov";
    contentTypes["video/x-ms-wmv"] = "wmv";
    contentTypes["video/x-flv"] = "flv";
    
    // Application formats
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
    
    // Web fonts
    contentTypes["font/woff"] = "woff";
    contentTypes["font/woff2"] = "woff2";
    contentTypes["font/ttf"] = "ttf";
    contentTypes["font/otf"] = "otf";
    contentTypes["application/vnd.ms-fontobject"] = "eot";
    
    // Programming and configuration
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

bool PostHandler::directoryExists(const std::string& path) {
    struct stat info;
    
    if (stat(path.c_str(), &info) != 0) {
        // std::cout << "is not a dir\n";
        return false;
    }
    
    // std::cout << "is a dir\n";
    return true;
}

std::string PostHandler::createUniqueFile(const std::string& extension, std::string& location_path) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::ostringstream filename;

    // std::cout << "Debug - location_path: '" << location_path << "'" << std::endl;
    if (directoryExists(location_path))
    {
        if (!location_path.empty() && location_path[location_path.length() - 1] != '/' ) {
            // std::cout << "herererere \n";
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
        status = 404;
        // std::cout << "directory not found\n";
        return "";
    }
    
    // std::cout << "file name " << filename.str() << std::endl;
    if (!extension.empty()) {
        filename << "." << extension;
    }
    

    std::ofstream outfile(filename.str().c_str(), std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file: " << filename.str() << std::endl;
        return "";
    }
    outfile.close();
    return filename.str();
}


void PostHandler::initialize(ParsRequest &data_req, ConfigParser &parser) {

    std::string contentType = "";
    size_t contentLength = 0;

    std::map<std::string, std::string> headers = data_req.getHeaders();
   
    std::map<std::string, std::string>::iterator contentTypeIt = headers.find("Content-Type");
    if (contentTypeIt != headers.end()) {
        contentType = contentTypeIt->second;
    }
    if (!data_req.isChunked())
    {
        std::map<std::string, std::string>::iterator contentLengthIt = headers.find("Content-Length");
        if (contentLengthIt != headers.end()) {
            contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 10);
            std::cout << "contentlenght " << contentLength << std::endl;
        }
    }
    this->expectedLength = contentLength;
    this->bodyLength = 0;
    this->isComplete = false;
    this->body = "";
    Server server = parser.getServer(data_req.hostMethod(), data_req.portMethod());
    
    locations = server.getLocations();
    std::string location_path = "";
    Location location ;
    std::map<std::string, Location>::iterator locationIt = locations.find(data_req.getPath());
    if (locationIt != locations.end())
    {
        location = locationIt->second;
        location_path =location.getRoot();

    }
    else{
        std::cout << "location not found\n";
        status = 404;
        return ;
    }
     std::vector<std::string> allow_methods = location.getMethods();
    if (std::find(allow_methods.begin(), allow_methods.end(), "POST") != allow_methods.end()) {
        // std::cout << "POST is allowed." << std::endl;
        std::cout << location_path << std::endl;
        status = 200;
        this->maxBodySize = server.getClientMaxBodySize();
        
        std::string extension = "";
        std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
        if (itT != contentTypes.end()) {
            extension = itT->second;
        } else {
            std::cout << "Extension not found for content type: " << contentType << std::endl;
        }
        if (expectedLength > maxBodySize && maxBodySize > 0 && expectedLength > 0){
            std::cout << "This file has a content lenght greater then max body size : " << std::endl;
            isComplete = true;
            return;
        }else{
            this->filename = createUniqueFile(extension, location_path);
        }
        if (!filename.empty()) {
            std::cout << "Created file: " << filename << std::endl;
        }
        else{
            return;
        }
        file.open(filename.c_str(), std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
        }
        this->isChunked = data_req.isChunked();
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
                std::cout << "here processData " << std::endl;
            }
        }

    }else {
        std::cout << "POST is not allowed." << std::endl;
        status = 405;
        return;
    }
    


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
    (void)data_req;
    (void)location_path;
    
    initBody = this->leftoverData + initBody;
    std::string body = initBody;
    size_t tPos = 0;
    size_t sPos = 0;
    size_t hPos = 0;
    this->leftoverData = "";

    // Find terminator
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
                // fetchData();
                body = body.substr(sep.length(), body.length());
                if ((hPos = body.find("\r\n\r\n")) != std::string::npos){
                    this->headers = body.substr(0 , hPos);
                    std::string contentType = extractContentType(this->headers);
                    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
                    if (itT != contentTypes.end()) {
                        extension = itT->second;
                    } else {
                        std::cout << "Extension not found for content type: " << contentType << std::endl;
                    }
                    std::cout << "extension " << this->extension << std::endl;
                    if (this->file.is_open())
                    {
                        this->file.flush();
                        this->file.close();
                    }
                    this->filename = createUniqueFile(extension, location_path);
                    if (!filename.empty()) {
                        std::cout << "Created file: " << filename << std::endl;
                    }
                    else{
                        return;
                    }
                    file.open(filename.c_str(), std::ios::binary);
                    if (!file) {
                        std::cerr << "Failed to open file for writing: " << filename << std::endl;
                    }
                    body = body.substr(hPos + 4, body.length());
                }
                
            }
            processBoundaryData(body, data_req, location_path);
        }else if ((sPos = body.find(sep)) == std::string::npos){
            if (check == 0){
                if (body.length() > sep.length())
                {
                    // std::cout << "this scope 2\n";
                    this->leftoverData = body.substr(body.length() - sep.length(), body.length());
                    this->file << body.substr(0, body.length() - sep.length());
                    
                }else{
                    // std::cout << "this scope 3\n";
                    this->leftoverData = body;
                }
            }else{
                
                // std::cout << "this scope 1\n";
                this->file << body;
            }
        }
        if (this->check == 1)
        {
            // std::cout << " this check ==>  "  << std::endl;

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
    
    // std::cout << "{ BODY " << initBody << " }" << std::endl;
    // std::cout << "{ BOUNDARY VALUE " << boundaryValue << " }" << std::endl;
    Server server = parser.getServer(data_req.hostMethod(), data_req.portMethod());
    locations = server.getLocations();
    
    std::string location_path = "";
    Location location;

    std::map<std::string, Location>::iterator locationIt = locations.find(data_req.getPath());
    if (locationIt != locations.end()) {
        location = locationIt->second;
        location_path = location.getRoot();
    } else {
        std::cout << "Location not found: " << data_req.getPath() << std::endl;
        status = 404;
        return;
    }

    std::vector<std::string> allow_methods = location.getMethods();
    if (std::find(allow_methods.begin(), allow_methods.end(), "POST") != allow_methods.end()) {
    
        std::string body = initBody; 
        processBoundaryData(body, data_req, location_path);
    } else {
        std::cout << "Method POST not allowed\n";
        status = 405;
        return;
    }
}








void PostHandler::processData(const std::string& data) {
    if (!file.is_open()) {
        std::cerr << "Error: File is not open when trying to process data" << std::endl;
        return;
    }
    
    file << data;

    // body += data;

    bodyLength += data.length();

    if (bodyLength >= expectedLength) {
        file.flush();
        file.close();
        isComplete = true;
        std::cout << "File upload complete: " << filename << " (" << bodyLength << " bytes)" << std::endl;
    }
}

void PostHandler::processChunkedData(const std::string& data) {
    size_t pos = 0;
    while (pos < data.length())
    {
        switch (chunkState)
        {
            case READING_SIZE:{
                size_t endPos = data.find("\r\n", pos);
                if (endPos == std::string::npos) {
                    chunkSizeBuffer.append(data.substr(pos));
                    pos = data.length();
                    // std::cout << "chunkSizeBuffer ===> " << chunkSizeBuffer << std::endl;
                }
                else{
                    chunkSizeBuffer.append(data.substr(pos, endPos - pos));
                    // std::cout << "chunkSizeBuffer +++++> " << chunkSizeBuffer << std::endl;
                    std::string sizeStr = chunkSizeBuffer;

                    // convert hex to int
                    std::istringstream ss(sizeStr);
                    int size;
                    ss >> std::hex >> size;
                    // std::cout << "int =>" << size << std::endl;

                    if (size == 0){
                        chunkState = END_OF_CHUNKS;
                        isComplete = true;
                        file.flush();
                        file.close();
                        std::cout << "Chunked upload complete: " << filename << " (" << bodyLength << " bytes)" << std::endl;
                    }else{
                        currentChunkSize = size;
                        currentChunkBytesRead = 0;
                        chunkState = READING_DATA;
                    }
                    pos = endPos + 2;
                    chunkSizeBuffer.clear();
                }
                break;
            }
            case READING_DATA:{

                int bytesRemaining = currentChunkSize - currentChunkBytesRead;
                // std::cout << "data lenght " << data.length() << " bytesRemaining " << bytesRemaining << std:: endl;
                int bytesToRead = std::min((int)(data.length() - pos), bytesRemaining);

                if (bytesToRead > 0)
                {
                    file.write(data.c_str() + pos, bytesToRead);
                    bodyLength += bytesToRead;
                    currentChunkBytesRead += bytesToRead;
                    pos += bytesToRead;
                }
                if (currentChunkBytesRead >= currentChunkSize)
                {
                    chunkState = READING_TRAILING_CRLF;
                }
                break;

            }
            case READING_TRAILING_CRLF: {
                if (data.length() - pos >= 2) {
                    pos += 2; 
                    chunkState = READING_SIZE;
                } else {
                    pos = data.length();
                }
                break;
            }
            case END_OF_CHUNKS:
                pos = data.length();
                break;
        }

    }
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