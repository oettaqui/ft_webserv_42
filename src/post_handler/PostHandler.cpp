#include "PostHandler.hpp"

PostHandler::PostHandler() : bodyLength(0), expectedLength(0), isComplete(false) {
    storeContentTypes();
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

void PostHandler::initialize(const std::string& contentType, size_t expectedContentLength, const std::string& initialBody, bool isChunkedTransfer) {
    this->expectedLength = expectedContentLength;
    this->bodyLength = 0;
    this->isComplete = false;
    this->body = "";
    

    std::string extension = "";
    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
    if (itT != contentTypes.end()) {
        extension = itT->second;
    } else {
        std::cout << "Extension not found for content type: " << contentType << std::endl;
    }


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

    this->isChunked = isChunkedTransfer;
    if (isChunked) {
        this->expectedLength = 0;
        this->chunkState = READING_SIZE;
        this->currentChunkSize = -1;
        this->currentChunkBytesRead = 0;
        this->chunkSizeBuffer = "";
    }
    

    if (!initialBody.empty()) {
        if (isChunked) {
            processChunkedData(initialBody);
            // std::cout << "here" << std::endl;
        } else {
            processData(initialBody);
        }
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