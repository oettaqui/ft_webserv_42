#include "PostHandler.hpp"

PostHandler::PostHandler() : bodyLength(0), expectedLength(0), isComplete(false){
    storeContentTypes();
    maxBodySize = 0;
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

std::string PostHandler::createUniqueFile(const std::string& extension) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    

    std::ostringstream filename;
    filename << "file_" << tv.tv_sec << "_" << tv.tv_usec;
    

    if (!extension.empty()) {
        filename << "." << extension;
    }
    

    std::ofstream outfile(filename.str().c_str(), std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file: " << filename.str() << std::endl;
        return "";
    }
    // std::cout << "extension from the function unique " << extension << std::endl;
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
    // std::cout << "max body size ===>> " << server.getClientMaxBodySize() << std::endl;
    // check if the location is exist in the server if not i should generate a error res depend on the RFC doc
    // check is the method post is availabl on this location if not i should generate a error res depend on the RFC doc
    // 
    this->maxBodySize = server.getClientMaxBodySize();
    std::string extension = "";
    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
    if (itT != contentTypes.end()) {
        extension = itT->second;
    } else {
        std::cout << "Extension not found for content type: " << contentType << std::endl;
    }

    if (expectedLength > maxBodySize){
        std::cout << "This file has a content lenght greater then max body size : " << std::endl;
        isComplete = true;
        return;
    }else
        this->filename = createUniqueFile(extension);
    if (!filename.empty()) {
        std::cout << "Created file: " << filename << std::endl;
    }
    

    if (file.is_open()) {
        file.close();
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
            // std::cout << "here" << std::endl;
        } else {
            processData(data_req.getBody());
            std::cout << "here processData " << std::endl;
        }
    }
}

void PostHandler::initBoundary(const std::string& initBody, const std::string &boundaryValue){
    this->bodyLength = 0;
    this->isComplete = false;
    this->body = "";

    // Example for file request
    //     Content-Disposition: form-data; name=""; filename="ft_transcendence.pdf"
    //     Content-Type: application/pdf
    // 
    //     starting data...
    // Example for just input type text
    //     Content-Disposition: form-data; name="here1"
    //
    //     ffffff1
        // (void)boundaryValue;
    if (initBody.find("Content-Type:") != std::string::npos)
    {
        std::cout << "This part contains a file or non-text content" << std::endl;
    }
    else
    {
        // let's work on the text request that finish there data from the first read (1024)
        std::cout << "This part is likely a simple text field" << std::endl;
         std::string extension = "txt";
        this->filename = createUniqueFile(extension);
        if (!filename.empty()) {
            std::cout << "Created file: " << filename << std::endl;
        }
        

        if (file.is_open()) {
            file.close();
        }
        
        file.open(filename.c_str(), std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
        }
        if (!initBody.empty()) {
            processBoundaryData(initBody, boundaryValue);
        }
         
    }
    // std::cout << "body ==> " << initBody << std::endl;
    // std::cout << "boundary value" << boundaryValue << std::endl;
    

}



std::string PostHandler::extractFormFieldValue(const std::string& body, const std::string& boundary) {
    std::string result = "";
    std::string tmp = "";
    std::string tmp1 = "";
    size_t startPos = 2;
    size_t end ;
    std::string terminator = "--" + boundary + "--";


    end = body.find(terminator);
    if (end == std::string::npos){
        std::cout << "terminator not found " << end << std::endl;
    }else{
        // std::cout << "terminator found " << end << std::endl;
        startPos += boundary.length() + 2;
        tmp = body.substr(startPos, end - startPos);
        // std::cout << boundary.length() << std::endl;
        // std::cout << "first substr ===> " << tmp << std::endl;
        startPos = tmp.find("\r\n\r\n");
        if (startPos == std::string::npos)
        {
            std::cout << "something went rong " << std::endl;
        }
        else{
            result = tmp.substr(startPos + 4);
            end = result.find("\n");
            if (end != std::string::npos)
            {
                result = result.substr(0, end - 1);
            }
        }

    }
    return result;

}

void PostHandler::processBoundaryData(const std::string &data, const std::string &boundaryValue){
    if (!file.is_open()) {
        std::cerr << "Error: File is not open when trying to process data" << std::endl;
        return;
    }

    std::string value = extractFormFieldValue(data, boundaryValue);
    if (value.empty())
    {
        std::cout << "i should check the value is empty " << std::endl;
    }
    // std::cout << "value ==>" << value << "|" << std::endl;
    file << value;
    file.flush();
    file.close();
    isComplete = true;


}


void PostHandler::processData(const std::string& data) {
    if (!file.is_open()) {
        std::cerr << "Error: File is not open when trying to process data" << std::endl;
        return;
    }
    
    file << data;
    // std::cout << "write on the file " << std::endl;
    // body += data;

    bodyLength += data.length();
    // // std::cout << "body len " << bodyLength << " expectedLength " << expectedLength << std::endl;
    // if (bodyLength >= maxBodySize )
    // {
    //     file.flush();
    //     file.close();
    //     isComplete = true;
    //     std::cout << "This file has a content lenght greater then max body size : " << filename << " (" << bodyLength << " bytes)" << std::endl;
    //     return;
    // }
    // std::cout << "body lenght ==> " << bodyLength << " | max body size ==> " << maxBodySize << std::endl;
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