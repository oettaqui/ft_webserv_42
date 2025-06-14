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
        return false;
    }
    // return true;
    return S_ISDIR(info.st_mode);
}

std::string PostHandler::createUniqueFile(const std::string& extension, std::string& location_path) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::ostringstream filename;

    std::cout << "Debug - location_path: '" << location_path << "'" << std::endl;
    
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
        status = 403;
        std::cout << "\n";
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


bool PostHandler::fileExistsAndNotEmpty(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file && file.peek() != std::ifstream::traits_type::eof();
}

std::string PostHandler::getTheValidIndex(std::vector<std::string> index, std::string path) {
    std::cout << "*****======== PATH ======*****" << std::endl;
    std::cout << path<< std::endl;
    std::cout << "*****======== PATH ======*****" << std::endl;


    if (path.empty() || path[path.length() - 1] != '/') {
        path += "/";
    }

    std::string tmp;
    std::vector<std::string>::iterator it = index.begin();
    while (it != index.end()) {
        tmp = path + *it;
        std::cout << "index ===> " << *it << std::endl;
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
            std::cout << "contentlenght " << contentLength << std::endl;
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
    std::cout << "************* CORRECT PATH ( " << correctPath << " )*****************" << std::endl;
    size_t Ppos = correctPath.find(".");
    if ( Ppos != std::string::npos && Ppos + 1 < correctPath.length() && correctPath[Ppos + 1] != '/' && Ppos == 0)
    {
        correctPath = correctPath.substr(0, Ppos + 1) + "/" + correctPath.substr(Ppos + 1, correctPath.length());  
        std::cout << "************* CORRECT PATH UPDATE ( " << correctPath << " )*****************111" << std::endl;
    }
    if (correctPath.empty()){
        std::cout << "Location Not found 404 \n";
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
        std::cout << "hnaaaa2222222222222222222222222222222222222222\n";
        this->scriptPath = p;
        c = 1;
        fp = p.find_last_of("/", p.length());
        l = p.substr(0, fp);
        fileN = p.substr(fp + 1, p.length());
    }
    else if (p.find_last_of(".", p.length()) != std::string::npos)
        c = 2;
    std::cout << "P " << p << std::endl;
    location = LocationAndPath.second;
    std::vector<std::string> allow_methods = location.getMethods();
    if(std::find(allow_methods.begin(), allow_methods.end(), "POST") == allow_methods.end())
    {
        std::cout << "POST is not allowed." << std::endl;
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
        // if (isChunked)
        // {
        //     std::cout << "=============  IS CGI and CHUNKED ========\n";
        //     this->status = 400;
        //     std::cout << "is a CGI but the Chunked cannot support the CGI  \n";
        //     return;
        // }
        this->cgiPassMap = location.getCgiPass();
        
        this->cgi_pass = location.getCgiPass();
        this->autoIndex = location.getAutoindex();
        std::cout << "=============  IS CGI ========" << isChunked << std::endl;
        if (c != 1)
        {
            location_path = correctPath;
            std::cout << "if111 " <<location_path << std::endl;
        }
        else{
            location_path = l;
            std::cout << "else " <<location_path << std::endl;
        }
        // if (c == 1)
        // {
        //     // if (this->autoIndex)
        //     // {
        //         std::cout << "iffffff " <<location_path << std::endl;
        //         sleep(3);
        //         // // indexV = location.getIndex();
                
        //         // // std::string tmp = getTheValidIndex(indexV, location_path);
        //         // std::string tmp = location_path.substr(location_path.find_last_of('/') + 1, location_path.length());
        //         // std::cout << "TMP ========= " << tmp << std::endl;
        //         // if (!tmp.empty() && (tmp.find(".php") != std::string::npos || tmp.find(".py") != std::string::npos || tmp.find(".perl") != std::string::npos))
        //         // {
        //         //     location_path = tmp;
        //         //     this->scriptPath = location_path;
        //         //     c = 1;
        //         //     fp = location_path.find_last_of("/", p.length());
        //         //     l = location_path.substr(0, fp);
        //         //     fileN = location_path.substr(fp + 1, location_path.length());
        //         // }
        //         // else if (tmp.empty() || (tmp.find(".php") == std::string::npos && tmp.find(".py") == std::string::npos && tmp.find(".perl") == std::string::npos)){
        //         //     this->status = 404;
        //         //     std::cout << "is a CGI but i don't have the extension that i should interprete it \n";
        //         //     return;
        //         // }
        //     // }
        //     // else{}



        // }
        if (c != 1){
            std::cout << "elseeee " <<location_path << std::endl;
            // sleep(10);
            if (this->autoIndex)
            {
                indexV = location.getIndex();
                
                std::string tmp = getTheValidIndex(indexV, location_path);
                std::cout << "TMP ========= " << tmp << std::endl;
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
                    std::cout << "is a CGI but i don't have the extension that i should interprete it \n";
                    return;
                }
            }
            else{
                std::cout << "PPPPPPPPPPPP\n";
                this->isCGI = false;
                this->status = 403;
                return;
            }

        }
    }else{
        // std::cout << "=============  IS NOT CGI ========\n";
        this->isCGI = false;
        c = 2;
    }
    if (c != 1)
    {
        location_path = correctPath;
        std::cout << "if " << location_path << std::endl;
    }
    else{
        location_path = l;
        std::cout << "else " <<location_path << std::endl;
    }

    std::cout << "Location Path" << location_path << std::endl;
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
                {
                    this->extension = fileN.substr(pointP + 1, fileN.length());
                    std::cout << "extension " << extension << std::endl;
                }
                else{
                    std::cout << "==============> " << fileN << std::endl;
                }
            }
        }
        else
            std::cout << "Content Type Unsupported" << std::endl;
            // shoudl check this !
    }
    if (expectedLength > maxBodySize && maxBodySize > 0 && expectedLength > 0){
        std::cout << "This file has a content lenght greater then max body size : " << std::endl;
        std::cout << "Payload Too Large (413)" << std::endl;
        isComplete = true;
        this->status = 413;
        return;
    }else{
        this->filename = createUniqueFile(extension, location_path);
        if (status == 403)
        {
            std::cout << "403!!!!\n";
            return;
        }
        if (filename.empty()) {
            std::cerr << "Failed to Create File (500)" << std::endl;
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
            std::cout << "here processChunkedData " << std::endl;
            processChunkedData(data_req.getBody());
        } else {
            processData(data_req.getBody());
            std::cout << "here processData " << std::endl;
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
    (void)data_req;
    (void)location_path;
    
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
                    } else {
                        std::cout << "Extension not found for content type: " << contentType << std::endl;
                        // extension = "txt";
                    }
                    std::cout << "extension " << this->extension << std::endl;
                    if (this->file.is_open())
                    {
                        this->file.flush();
                        this->file.close();
                    }
                    this->filename = createUniqueFile(extension, location_path);
                    if (this->status == 404)
                        return;
                    if (!filename.empty()) {
                        std::cout << "Created file: " << filename << std::endl;
                    }
                    else{
                        this->status = 500;
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
    

    Server server = parser.getServer(data_req.hostMethod(), data_req.portMethod());
    locations = server.getLocations();
    
    // std::string location_path = "";
    // Location location;

    // std::map<std::string, Location>::iterator locationIt = locations.find(data_req.getPath());
    // if (locationIt != locations.end()) {
    //     location = locationIt->second;
        std::string correctPath = "";
        std::pair<std::string, Location> LocationAndPath;

        LocationAndPath = getCorrectPath(locations, data_req.getPath());
        correctPath = LocationAndPath.first;
        std::cout << "************* " << correctPath << " *****************" << std::endl;
        if (correctPath.empty()){
            std::cout << "Location Not found 404 \n";
            this->status = 404;
            return;
        }
        std::string location_path = "";
        Location location ;
        // std::string p = correctPath;
        location = LocationAndPath.second;
        if (location.getCgi())
        {
            if (data_req.isBoundary())
            {
                std::cout << "=============  IS CGI and Boundary ========\n";
                this->status = 400;
                std::cout << "is a CGI but the boundaty cannot support the CGI  \n";
                return;
            }
            std::cout << "=============  IS CGI FROM BOUNDARY ========\n";
            this->isCGI = true;
            this->cgi_pass = location.getCgiPass();
            this->autoIndex = location.getAutoindex();
            status = 400;
            return;

        }else{
            // std::cout << "=============  IS NOT CGI FROM BOUNDARY ========\n";
            this->isCGI = false;
        }
        location_path = correctPath;
    
    //  else {
    //     std::cout << "Location not found: " << data_req.getPath() << std::endl;
    //     status = 404;
    //     return;
    // }

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

// void PostHandler::processChunkedData(const std::string& data) {
//     size_t pos = 0;
//     while (pos < data.length())
//     {
//         switch (chunkState)
//         {
//             case READING_SIZE:{
//                 size_t endPos = data.find("\r\n", pos);
//                 if (endPos == std::string::npos) {
//                     chunkSizeBuffer.append(data.substr(pos, data.length()));
//                     pos = data.length();
//                     // std::cout << "chunkSizeBuffer ===> " << chunkSizeBuffer << std::endl;
//                 }
//                 else{
//                     chunkSizeBuffer.append(data.substr(pos, endPos - pos));
//                     // std::cout << "chunkSizeBuffer +++++> " << chunkSizeBuffer << std::endl;
//                     std::string sizeStr = chunkSizeBuffer;

                    
//                     std::istringstream ss(sizeStr);
//                     int size;
//                     ss >> std::hex >> size;
//                     std::cout << "int =>" << size << std::endl;

//                     if (size == 0){
//                         chunkState = END_OF_CHUNKS;
//                         isComplete = true;
//                         file.flush();
//                         file.close();
//                         std::cout << "Chunked upload complete: " << filename << " (" << bodyLength << " bytes)" << std::endl;
//                     }else{
//                         currentChunkSize = size;
//                         currentChunkBytesRead = 0;
//                         chunkState = READING_DATA;
//                     }
//                     pos = endPos + 2;
//                     chunkSizeBuffer.clear();
//                 }
//                 break;
//             }
//             case READING_DATA:{

//                 int bytesRemaining = currentChunkSize - currentChunkBytesRead;
//                 // std::cout << "data lenght " << data.length() << " bytesRemaining " << bytesRemaining << std:: endl;
//                 int bytesToRead = std::min((int)(data.length() - pos), bytesRemaining);

//                 if (bytesToRead > 0)
//                 {
//                     file.write(data.c_str() + pos, bytesToRead);
//                     bodyLength += bytesToRead;
//                     currentChunkBytesRead += bytesToRead;
//                     pos += bytesToRead;
//                 }
//                 if (currentChunkBytesRead >= currentChunkSize)
//                 {
//                     chunkState = READING_TRAILING_CRLF;
//                 }
//                 break;

//             }
//             case READING_TRAILING_CRLF: {
//                 if (data.length() - pos >= 2) {
//                     pos += 2; 
//                     chunkState = READING_SIZE;
//                 } else {
//                     pos = data.length();
//                 }
//                 break;
//             }
//             case END_OF_CHUNKS:
//                 pos = data.length();
//                 break;
//         }

//     }
// }


void PostHandler::processChunkedData(const std::string& data) {
    // Append new data to the buffer
    buffer.append(data);
    
    // Process the buffered data
    processBufferedData();
}


void PostHandler::processBufferedData() {
    size_t pos = bufferOffset;
    
    while (pos < buffer.length()) {
        switch (chunkState) {
            case READING_SIZE: {
                size_t endPos = buffer.find("\r\n", pos);
                if (endPos == std::string::npos) {
                    chunkSizeBuffer = buffer.substr(pos); // Replace instead of append
                    bufferOffset = pos;
                    return;
                }
                
                chunkSizeBuffer = buffer.substr(pos, endPos - pos); // Replace instead of append
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
                
                // Add protection against zero-length reads
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
                
                // Verify it's actually CRLF
                if (buffer[pos] != '\r' || buffer[pos+1] != '\n') {
                    // Handle protocol error
                    chunkState = END_OF_CHUNKS;
                    isComplete = true;
                    file.close();
                    std::cerr << "Invalid chunk trailing CRLF" << std::endl;
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
        
        // Clean processed data from buffer periodically
        if (pos > 1024) {  // Arbitrary threshold
            buffer.erase(0, pos);
            pos = 0;
            bufferOffset = 0;
        }
    }
    
    // Clear buffer if we processed everything
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
            
            
            std::cout << "============ ROOT " << root << std::endl; 
            std::cout << "============ RESULT " << result << std::endl; 
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

    std::cout << "No matching location found.\n";
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


////////////// new block

std::string PostHandler::createRedirectResponse(int statusCode, const std::string& location) {
    std::stringstream response;
    
    response << "HTTP/1.1 " << statusCode << " ";
    
    // Add status text
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