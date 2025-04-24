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
    contentTypes["image/jpeg"] = "jpg";
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
    // Get current time with microsecond precision
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert seconds and microseconds to a string
    std::ostringstream filename;
    filename << "file_" << tv.tv_sec << "_" << tv.tv_usec;
    
    // Add the extension
    if (!extension.empty()) {
        filename << "." << extension;
    }
    
    // Create the file
    std::ofstream outfile(filename.str().c_str(), std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to create file: " << filename.str() << std::endl;
        return "";
    }
    
    // Close the file
    outfile.close();
    
    // Return the filename
    return filename.str();
}

void PostHandler::initialize(const std::string& contentType, size_t expectedContentLength, const std::string& initialBody) {
    this->expectedLength = expectedContentLength;
    this->bodyLength = 0;
    this->isComplete = false;
    this->body = "";
    
    // Determine the file extension based on content type
    std::string extension = "";
    std::map<std::string, std::string>::iterator itT = contentTypes.find(contentType);
    if (itT != contentTypes.end()) {
        extension = itT->second;
    } else {
        std::cout << "Extension not found for content type: " << contentType << std::endl;
    }

    // Create a unique file
    this->filename = createUniqueFile(extension);
    if (!filename.empty()) {
        std::cout << "Created file: " << filename << std::endl;
    }
    
    // Open the file for writing
    if (file.is_open()) {
        file.close();
    }
    
    file.open(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
    }
    
    // Process initial body content if any
    if (!initialBody.empty()) {
        processData(initialBody);
    }
}

void PostHandler::processData(const std::string& data) {
    if (!file.is_open()) {
        std::cerr << "Error: File is not open when trying to process data" << std::endl;
        return;
    }
    
    // Write to file
    file << data;
    
    // // Update body and length
    body += data;

    // file.write(data.c_str(), data.length());
    bodyLength += data.length();
    
    // Check if complete
    if (bodyLength >= expectedLength) {
        file.flush();
        file.close();
        isComplete = true;
        std::cout << "File upload complete: " << filename << " (" << bodyLength << " bytes)" << std::endl;
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