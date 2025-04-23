#include "./ParsRequest.hpp"

#include "./ParsRequest.hpp"

ParsRequest::ParsRequest(){
    header_parsed = false;
    bodyLength = 0;
    is_Complet = false;
    storeTypes();
    // file.open("/home/oettaqui/Desktop/ft_webser_42/file.jpeg");
    
}

void ParsRequest::storeTypes() {
         // Text formats
    types["text/html"] = "html";
    types["text/css"] = "css";
    types["text/xml"] = "xml";
    types["text/plain"] = "txt";
    types["text/markdown"] = "md";
    types["text/javascript"] = "js";
    types["application/json"] = "json";
    types["text/csv"] = "csv";
    
    // Image formats
    types["image/gif"] = "gif";
    types["image/jpeg"] = "jpg";
    types["image/png"] = "png";
    types["image/webp"] = "webp";
    types["image/svg+xml"] = "svg";
    types["image/x-icon"] = "ico";
    types["image/bmp"] = "bmp";
    types["image/tiff"] = "tiff";
    
    // Audio formats
    types["audio/mpeg"] = "mp3";
    types["audio/wav"] = "wav";
    types["audio/ogg"] = "ogg";
    types["audio/mp4"] = "m4a";
    types["audio/aac"] = "aac";
    types["audio/flac"] = "flac";
    types["audio/midi"] = "mid";
    
    // Video formats
    types["video/mp4"] = "mp4";
    types["video/webm"] = "webm";
    types["video/x-msvideo"] = "avi";
    types["video/mpeg"] = "mpg";
    types["video/quicktime"] = "mov";
    types["video/x-ms-wmv"] = "wmv";
    types["video/x-flv"] = "flv";
    
    // Application formats
    types["application/pdf"] = "pdf";
    types["application/msword"] = "doc";
    types["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = "docx";
    types["application/vnd.ms-excel"] = "xls";
    types["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = "xlsx";
    types["application/vnd.ms-powerpoint"] = "ppt";
    types["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = "pptx";
    types["application/zip"] = "zip";
    types["application/gzip"] = "gz";
    types["application/x-tar"] = "tar";
    types["application/x-rar-compressed"] = "rar";
    types["application/x-7z-compressed"] = "7z";
    types["application/x-msdownload"] = "exe";
    types["application/x-shockwave-flash"] = "swf";
    
    // Web fonts
    types["font/woff"] = "woff";
    types["font/woff2"] = "woff2";
    types["font/ttf"] = "ttf";
    types["font/otf"] = "otf";
    types["application/vnd.ms-fontobject"] = "eot";
    
    // Programming and configuration
    types["application/x-httpd-php"] = "php";
    types["text/x-python"] = "py";
    types["text/x-java-source"] = "java";
    types["text/x-c"] = "c";
    types["text/x-c++"] = "cpp";
    types["text/x-ruby"] = "rb";
    types["application/x-sh"] = "sh";
    types["text/x-perl"] = "pl";
    types["application/sql"] = "sql";
    types["application/xml"] = "xml";
    types["text/yaml"] = "yaml";
    types["application/octet-stream"] = "";
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
    std::vector<std::string> parts = split(it->second, ':');
    if (parts.size() == 2) {
        host = parts[0];
        port = parts[1];
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
        
        // if (!body.empty()) {
        //     // file << 
        //     std::cout << "\n=== Body ===\n" << body << std::endl;
        // }
}


// create a unique file

std::string ParsRequest::createUniqueFile(const std::string &extension){
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

void ParsRequest::parse(const std::string& request){
    
 
    requestContent += request;
    size_t header_end = requestContent.find("\r\n\r\n");
    if (header_end == std::string::npos && !header_parsed) {
        return;
    }
    else if (!header_parsed){
        // std::cout << request << std::endl;
        header_parsed = true;
        std::string header_section = requestContent.substr(0, header_end);
        body = requestContent.substr(header_end + 4);
        // std::cout << "| " << header_section << " |" << std::endl;
        std::vector<std::string> lines = split(header_section, '\n');
        if (lines.empty()) {
            return;
        }
        parseRequestLine(lines[0]);

        std::string headers_section;
        for (size_t i = 1; i < lines.size(); ++i) {
            headers_section += lines[i] + "\n";
        }
        parseHeaders(header_section);
        std::string extension = "";
        if (header_parsed)
        {
            std::map<std::string, std::string>::iterator it = headers.find("Content-Type");
            if (it != headers.end()) {
                // std::cout << "Content-Type: " << it->second << std::endl;
                std::map<std::string, std::string>::iterator itT = types.find(it->second);
                if (itT != types.end())
                {
                    extension = itT->second;
                    // std::cout << "extension name " << itT->second << std::endl;
                }else {
                    std::cout << "extension not found" << std::endl;
                }
            } else {
                std::cout << "Content-Type header not found" << std::endl;
            }

        }

        std::string filename = createUniqueFile(extension);
        if (!filename.empty()) {
            std::cout << "Created file: " << filename << std::endl;
        }
        if (file.is_open()) {
            file.close();
        }
        
        // Open the new file
        file.open(filename.c_str(), std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
        } else {
            // Write the body to the file
            requestContent = "";
            file << body;
            // file << body;
            // std::cout << "body.len " << body.length() << "\n" << body << std::endl;
            bodyLength = body.length();
        }
        if (header_parsed && bodyLength == std::strtoul(headers["Content-Length"].c_str(), NULL, 10))
        {
            // std::cout << "closed file " << bodyLength << " content len "<<  std::strtoul(headers["Content-Length"].c_str(), NULL, 10) << std::endl;
            file.close();
            is_Complet = true;
        }
    
    }
    else if (header_parsed){
        file << request;

        // std::cout << request << std::endl;

        // std::cout << bodyLength << "::" <<  std::strtoul(headers["Content-Length"].c_str(), NULL, 10) << std::endl;
        bodyLength += request.length();
        if (bodyLength == std::strtoul(headers["Content-Length"].c_str(), NULL, 10)){
            // std::cout << "closed file " << bodyLength << " content len "<<  std::strtoul(headers["Content-Length"].c_str(), NULL, 10) << std::endl;
            file.close();
            is_Complet = true;
            // std::cout << "file ===> " << file << std::endl;
            // exit(1);
        }
    }
    // printRequest();
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