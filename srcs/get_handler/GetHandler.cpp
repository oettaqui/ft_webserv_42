#include "GetHandler.hpp"
#include "unistd.h"

GetHandler::GetHandler() 
{
    check_put_header = 0;
    ingore_element = 0;
    check_if = 0;
    existent_folder = 0;
    statusCode = 200;
    status_message = "OK";
    totalBytesSent = 0;
    size = 0;
    is_true_parse = false;
    contentLength = 0;
}

GetHandler::~GetHandler() {
    // Destructor
}

void GetHandler::generate_header(int flag)
{
    if(check_put_header == 1)
        return ;
    std::cout << "=======================>\n";
    std::stringstream header;
    if(flag == 0)
    {
        std::cout << "header for small\n";
        header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
                << "Content-Type: " << contentType << "\r\n"
                << "Content-Length: " << contentLength << "\r\n"
                << "Connection: close\r\n"
                << "\r\n";
    }
    else
    {
        std::cout << "header for large\n";
        header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Transfer-Encoding: chunked\r\n"
        << "\r\n";
    }
    send(client_fd, header.str().c_str(), header.str().length(), 0);
    check_put_header = 1;
} 

size_t GetHandler::getFileSize(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string GetHandler::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return ""; // No extension found
}


void GetHandler::storeContentTypes(ParsRequest &request_data) {
    this->client_fd = request_data.getClientFd();
    // Text formats
    contentTypes["html"] = "text/html";
    contentTypes["css"] = "text/css";
    contentTypes["xml"] = "text/xml";
    contentTypes["txt"] = "text/plain";
    contentTypes["md"] = "text/markdown";
    contentTypes["js"] = "text/javascript";
    contentTypes["json"] = "application/json";
    contentTypes["csv"] = "text/csv";
    
    // Image formats
    contentTypes["gif"] = "image/gif";
    contentTypes["jpeg"] = "image/jpeg";
    contentTypes["jpg"] = "image/jpg";
    contentTypes["png"] = "image/png";
    contentTypes["webp"] = "image/webp";
    contentTypes["svg"] = "image/svg+xml";
    contentTypes["ico"] = "image/x-icon";
    contentTypes["bmp"] = "image/bmp";
    contentTypes["tiff"] = "image/tiff";
    
    // Audio formats
    contentTypes["mp3"] = "audio/mpeg";
    contentTypes["wav"] = "audio/wav";
    contentTypes["ogg"] = "audio/ogg";
    contentTypes["m4a"] = "audio/mp4";
    contentTypes["aac"] = "audio/aac";
    contentTypes["flac"] = "audio/flac";
    contentTypes["mid"] = "audio/midi";
    
    // Video formats
    contentTypes["mp4"] = "video/mp4";
    contentTypes["webm"] = "video/webm";
    contentTypes["avi"] = "video/x-msvideo";
    contentTypes["mpg"] = "video/mpeg";
    contentTypes["mov"] = "video/quicktime";
    contentTypes["wmv"] = "video/x-ms-wmv";
    contentTypes["flv"] = "video/x-flv";
    
    // Application formats
    contentTypes["pdf"] = "application/pdf";
    contentTypes["doc"] = "application/msword";
    contentTypes["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    contentTypes["xls"] = "application/vnd.ms-excel";
    contentTypes["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    contentTypes["ppt"] = "application/vnd.ms-powerpoint";
    contentTypes["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    contentTypes["zip"] = "application/zip";
    contentTypes["gz"] = "application/gzip";
    contentTypes["tar"] = "application/x-tar";
    contentTypes["rar"] = "application/x-rar-compressed";
    contentTypes["7z"] = "application/x-7z-compressed";
    contentTypes["exe"] = "application/x-msdownload";
    contentTypes["swf"] = "application/x-shockwave-flash";
    
    // Web fonts
    contentTypes["woff"] = "font/woff";
    contentTypes["woff2"] = "font/woff2";
    contentTypes["ttf"] = "font/ttf";
    contentTypes["otf"] = "font/otf";
    contentTypes["eot"] = "application/vnd.ms-fontobject";
    
    // Programming and configuration
    contentTypes["php"] = "application/x-httpd-php";
    contentTypes["py"] = "text/x-python";
    contentTypes["java"] = "text/x-java-source";
    contentTypes["c"] = "text/x-c";
    contentTypes["cpp"] = "text/x-c++";
    contentTypes["rb"] = "text/x-ruby";
    contentTypes["sh"] = "application/x-sh";
    contentTypes["pl"] = "text/x-perl";
    contentTypes["sql"] = "application/sql";
    contentTypes["xml"] = "application/xml";
    contentTypes["yaml"] = "text/yaml";
    contentTypes[""] = "application/octet-stream";
}

bool GetHandler::check_root(const std::string &value_p) const {
    if (value_p.length() >= 2 && value_p.substr(0, 2) == "./") {
        return true;
    }
    return false;
}

std::string GetHandler::generateAttractivePage(const std::vector<std::string>& items,const std::string &base_path) {
    std::string send_href;
    std::string path;
    DIR* dir = opendir(base_path.c_str());
    if (!dir) {
        statusCode = 404;
        status_message = "Not Found";
        check_if = 0;
        return "";
    }
    if(base_path != ".")
        path = trim(base_path,'.');
    std::cout << "generateAttractivePage\n";
    if(check_root(location_concerned.getRoot()))
    {
        std::cout << "hnaa1\n";
        send_href = location_base + path.substr(location_concerned.getRoot().length() - 1,path.length());
    }
    else if(path.empty())
    {
        send_href = base_path;
    }
    else
    {
        std::cout << "hnaa2\n";
        std::cout << "======================| location_base |============ :: " << location_base << std::endl;
        std::cout << "======================| base_path |============ :: " << base_path << std::endl;
        std::cout << "======================| path |============ :: " << path << std::endl;
        send_href = location_base + path.substr(location_concerned.getRoot().length(),path.length());
    }
    std::cout << "======================| (2)location_base |============ :: " << location_base << std::endl;
    std::cout << "======================| (2)path |============ :: " << path << std::endl;
    std::cout << "======================| (2)first send_href |============ :: " << send_href << std::endl;
    std::string html = "<!DOCTYPE html>\n"
                       "<html lang=\"en\">\n"
                       "<head>\n"
                       "    <meta charset=\"UTF-8\">\n"
                       "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                       "    <title>Files List</title>\n"
                       "    <style>\n"
                       "        body {\n"
                       "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
                       "            background-color: #f5f5f5;\n"
                       "            margin: 0;\n"
                       "            padding: 20px;\n"
                       "            color: #333;\n"
                       "        }\n"
                       "        .container {\n"
                       "            max-width: 800px;\n"
                       "            margin: 0 auto;\n"
                       "            background-color: white;\n"
                       "            padding: 30px;\n"
                       "            border-radius: 10px;\n"
                       "            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);\n"
                       "        }\n"
                       "        h1 {\n"
                       "            color: #2c3e50;\n"
                       "            border-bottom: 2px solid #3498db;\n"
                       "            padding-bottom: 10px;\n"
                       "            margin-top: 0;\n"
                       "        }\n"
                       "        .links-list {\n"
                       "            list-style-type: none;\n"
                       "            padding: 0;\n"
                       "        }\n"
                       "        .link-item {\n"
                       "            margin-bottom: 12px;\n"
                       "            padding: 12px;\n"
                       "            border-radius: 5px;\n"
                       "            background-color: #f9f9f9;\n"
                       "            transition: all 0.3s ease;\n"
                       "        }\n"
                       "        .link-item:hover {\n"
                       "            background-color: #eaf2f8;\n"
                       "            transform: translateX(5px);\n"
                       "        }\n"
                       "        .link-item a {\n"
                       "            color: #3498db;\n"
                       "            text-decoration: none;\n"
                       "            display: block;\n"
                       "            font-size: 16px;\n"
                       "        }\n"
                       "        .link-item a:hover {\n"
                       "            color: #2980b9;\n"
                       "            text-decoration: underline;\n"
                       "        }\n"
                       "        .footer {\n"
                       "            margin-top: 30px;\n"
                       "            text-align: center;\n"
                       "            color: #7f8c8d;\n"
                       "            font-size: 14px;\n"
                       "        }\n"
                       "    </style>\n"
                       "</head>\n"
                       "<body>\n"
                       "    <div class=\"container\">\n"
                       "        <h1>Files List</h1>\n"
                       "        <ul class=\"links-list\">\n";

    if(location_base == "/" && location_concerned.getRoot() != ".")
    {
        send_href = send_href.substr(1, send_href.length());
        std::cout << "======================| (3)first send_href |============ :: " << send_href << std::endl;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        if(isDirectory(base_path + "/" + items[i]))
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + send_href + "/" + items[i] + "\">"  + items[i] +" 📁</a>\n"
                    "            </li>\n";
        }
        else
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + send_href + "/" + items[i] + "\">"  +  items[i] + " 📄</a>\n"
                    "            </li>\n";         
        }
    }

    html += "        </ul>\n"
            "        <div class=\"footer\">\n"
            "            bchokri 42\n"
            "        </div>\n"
            "    </div>\n"
            "</body>\n"
            "</html>";
    // is_true_parse = true;
    contentLength = html.length();
    generate_header(0);
    return html;
}
std::vector<std::string> GetHandler::split(const std::string& str, char delim) const {
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

std::vector<std::string> GetHandler::listFiles(const std::string& dirPath) {
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* ent;

    dir = opendir(dirPath.c_str());
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                files.push_back(ent->d_name);
            }
        }
        closedir(dir);
    } 
    else
    {
        existent_folder = 1;
        std::cerr << "Could not open directory: " << dirPath << std::endl;
    }
    return files;
}

bool GetHandler::isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        std::cerr << "Error getting file information: " << path << std::endl;
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

std::vector<std::string> GetHandler::check_root_location(std::string directoryPath)
{
    std::vector<std::string> fileList = listFiles(directoryPath);

    if (!fileList.empty()) {
        std::cout << "Files in directory '" << directoryPath << "':" << std::endl;
        for (std::vector<std::string>::const_iterator it = fileList.begin(); it != fileList.end(); ++it) {
            std::cout << *it << std::endl;
            std::string fullPath = directoryPath + "/" + *it;
            if (isDirectory(fullPath)) {
                std::cout << "  (Directory)" << std::endl;
            } else {
                std::cout << "  (File)" << std::endl;
            }
        }
    } 
    else {
        std::cout << "No files found in directory '" << directoryPath << "'" << std::endl;
    }
    return (fileList);
}

std::string GetHandler::trim(const std::string& str, char ch) {
    std::string::size_type start = str.find_first_not_of(ch);
    std::string::size_type end = str.find_last_not_of(ch);

    if (start == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::string GetHandler::get_path_to_get()
{
    std::string tmp;
    size_t count = 0;
    tmp = location_concerned.getRoot();
    for (std::vector<std::string>::const_iterator it = path_location.begin(); it != path_location.end(); ++it) {
        if(count >= ingore_element)
            tmp = tmp + '/' + *it;
        count++;
    }
    return tmp;
}


std::vector<std::string> GetHandler::get_location_server() const
{
    std::string tmp;
    std::vector<std::string> find_vec;
    for (std::vector<std::string>::const_iterator it = path_location.begin(); it != path_location.end(); ++it) {
        tmp = tmp + '/' + *it;
        if(server_socket.getLocations().find(tmp) != server_socket.getLocations().end())
            find_vec.push_back(tmp);
    }
    return find_vec;
}

std::string GetHandler::handleGetRequest(ParsRequest &request_data,ConfigParser &parser) {
    storeContentTypes(request_data);
    path_location = this->split(url_decode(request_data.getPath()),'/');
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::cout << "\n\nhandleGetRequest\n";
    std::cout << "this is the path : ***|" <<  request_data.getPath() << "|***" << std::endl;
    std::cout << "this is the path decoded : ***|" <<  url_decode(request_data.getPath()) << "|***" << std::endl;
    std::vector<std::string> find_vec = get_location_server();
    if(find_vec.size() != 0)
    {
        std::cout << "loaction is  : ==>> " << find_vec.back() <<  std::endl;
    }
    else
        std::cout << " <<== noo loaction  ==>> " <<  std::endl;

    if(find_vec.size() != 0 )
    {
        std::cout << "/////////////////////////////////////////////\n";
        location_base = find_vec.back();
        it_find_location_server = server_socket.getLocations().find(location_base);
        location_concerned =  it_find_location_server->second;
        ingore_element = split(find_vec.back(),'/').size();
        index_file = get_path_to_get();
        std::cout << "index_file : " << index_file << std::endl;
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"GET") 
        != location_concerned.getMethods().end()))
        {
            statusCode = 403;
            status_message = "Forbidden";
            contentType = "text/html";
            contentLength = 54;
            generate_header(0);
            return generateResponse("<h1>403 the client doesn't have permission to GET</h1>", request_data);
        }
        else
        {
            if(location_concerned.getAutoindex() == true)
            {
                std::cout << "*************Autooooooooooooooooo indexxxxxxxxxxxxxxx**********\n";
                if(location_concerned.getIndex().size() != 0)
                {
                    index_file = location_concerned.getRoot() + '/' + *location_concerned.getIndex().begin();
                    content = readFile(index_file);
                    if(content.empty() && check_if == 0)
                    {
                        std::cout << "*************Autoo !content **********\n";
                        content = readFile("./default/index.html");
                    }
                }
                else
                   content = readFile("./default/index.html");
            }
            else if(!isDirectory(index_file))
            {
                content = readFile(index_file);
            }
            else if(isDirectory(index_file))
            {
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file);
            }
        }
    }
    else if(find_vec.size() == 0)
    {
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++\n";
        location_base = "/";
        // location_base = request_data.getPath();
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned =  it_find_location_server->second;
        index_file = get_path_to_get();
        std::cout << " find_vec.size() == 0 ==> index_file : " << index_file << std::endl;
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"GET") 
        != location_concerned.getMethods().end()))
        {
            contentType = "text/html";
            std::cout << "imposiiiii \n";
            statusCode = 403;
            status_message = "Forbidden";
            contentLength = 54;
            generate_header(0);
            return generateResponse("<h1>403 the client doesn't have permission to GET</h1>", request_data);
        }
        else
        {
            if(location_concerned.getAutoindex() == true && request_data.getPath() == "/")
            {
                std::cout << "*************Autooooooooooooooooo indexxxxxxxxxxxxxxx**********\n";
                if(location_concerned.getIndex().size() != 0)
                {
                    index_file = location_concerned.getRoot() + '/' + *location_concerned.getIndex().begin();
                    content = readFile(index_file);
                    if(content.empty() && check_if == 0)
                    {
                        std::cout << "*************Autoo !content **********\n";
                        content = readFile("./default/index.html");
                    }
                }
                else
                   content = readFile("./default/index.html");
            }
            else if(!isDirectory(index_file))
            {
                std::cout << "haaaaaaaaaaaaaaaaa1\n";
                content = readFile(index_file);
            }
            else if(isDirectory(index_file))
            {
                std::cout << "haaaaaaaaaaaaaaaaa2\n";
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file);
            }
            else
            {
                std::cout << "haaaaaaaaaaaaaaaaa3\n";
                content = "<h1>test 1</h1>";
                generate_header(0);
            }
        }
    }
    if (content.empty() && check_if == 0) {
        statusCode = 404;
        contentType = "text/html";
        status_message = "Not Found";
        std::cout << "aaaaaaaaaaaaaaaaaaaaaaaa\n";
        contentLength = 22;
        generate_header(0);
        return generateResponse("<h1>404 Not Found</h1>", request_data);
    }
    return generateResponse(content, request_data);
}

std::string GetHandler::readFile(const std::string& filePath) {
    std::string extension;
    if(!filePath.empty()) {
        extension = getFileExtension(filePath);
        std::map<std::string, std::string>::const_iterator it = contentTypes.find(extension);
        if(it != contentTypes.end() && !extension.empty()) {
            contentType = it->second;
        }
    }
    
    if(!file.is_open())
    {
        std::cout << "dkhal-------------\n";
        file.open(filePath.c_str(), std::ios::binary);
        if (!file) {
            std::cout << "failed Read ///////\n";
            return "";
        }
    }
    
    if(size == 0)
    {
        size = getFileSize(filePath);
        contentLength = size;
    }
    std::cout << "@@@@@@@@@@@@@ |" << size << "| @@@@@@@@@@@@\n";
    // char buffer[bufferSize];
    const ssize_t threshold = 1024*1024; // 1MB threshold
    bool useChunked = (size > threshold);
    generate_header(useChunked ? 1 : 0);
    
    if (!useChunked) {
        // Small file - send with Content-Length
        return readSmallFile(file);
    } else {
        // Large file - send chunked
        return readLargeFileChunked(file);
    }
}

std::string GetHandler::readSmallFile(std::ifstream& file) {
    std::string content;
    // content.reserve(size); // Pre-allocate
    char buffer[BUFFER_SIZE_G];
    memset(buffer, 0, BUFFER_SIZE_G);
    file.read(buffer, BUFFER_SIZE_G);
    content.append(buffer, file.gcount());
    
    // Send all at once for small files
    std::cout << "**********************************\n"; 
    std::cout << file.gcount() << std::endl;
    // std::cout << content << std::endl;
    // std::cout << buffer << std::endl;
    std::cout << "**********************************\n"; 
    ssize_t bytesSent = send(client_fd, content.c_str(), content.length(), 0);
    if (bytesSent < 0) {
        std::cout << "Send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    totalBytesSent = totalBytesSent + bytesSent;
    check_if = 1;
    if(totalBytesSent >= size)
    {
        std::cout << "++++++++++++++++++++++++++++++333333333\n";
        file.close();
        is_true_parse = true;
    }
    return "";
}

std::string GetHandler::readLargeFileChunked(std::ifstream& file) {
    char buffer[BUFFER_SIZE_G];
    memset(buffer, 0, BUFFER_SIZE_G);
    file.read(buffer, BUFFER_SIZE_G);
    size_t bytesRead = file.gcount();
    std::cout << "^^^^^^^^^^=> | " << bytesRead << " |<=^^^^^^^^^\n";
    
    // Send chunk size in hex + CRLF
    std::stringstream chunkSize;
    chunkSize << std::hex << bytesRead << "\r\n";
    
    ssize_t bytesSent = send(client_fd, chunkSize.str().c_str(), chunkSize.str().length(), 0);
    if (bytesSent < 0) {
        std::cout << "Chunk size send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    
    // Send chunk data
    bytesSent = send(client_fd, buffer, bytesRead, 0);
    if (bytesSent < 0) {
        std::cout << "Chunk data send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    
    // Send CRLF after chunk data
    bytesSent = send(client_fd, "\r\n", 2, 0);
    if (bytesSent < 0) {
        std::cout << "Chunk CRLF send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }    
    totalBytesSent += bytesRead;
         // Clear buffer
    // Send final chunk (0-sized chunk + CRLF + CRLF)
    if(totalBytesSent >= size)
    {
        const char* finalChunk = "0\r\n\r\n";
        ssize_t bytesSent = send(client_fd, finalChunk, 5, 0);
        if (bytesSent < 0) {
            std::cout << "Final chunk send error: " << strerror(errno) << std::endl;
            close(client_fd);
            file.close();
            return "";
        }
    }
    
    if(totalBytesSent >= size)
    {
        std::cout << "Chunked send complete. Total bytes sent: " << totalBytesSent << std::endl;
        file.close();
        std::cout << "++++++++++++++++++++++++++++++222222222222\n";
        is_true_parse = true;
    }
    check_if = 1;
    return "";
}

bool GetHandler::get_is_true_parse() const
{
    return is_true_parse;
}

std::string GetHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    std::stringstream response;
    (void)request_data;
    std::cout << "check_put_header : " << check_put_header << std::endl;
    std::cout << "check_if : " << check_if << std::endl;
    std::cout << "existent_folder : " << existent_folder << std::endl;
    if (!content.empty())
    {
        std::cout << "trueeeeeeeeeeeeee\n";
        is_true_parse = true;
    }
    response << content;
    return response.str();
}

std::string GetHandler::url_decode(std::string url) {
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%') {
            int hex = strtol(url.substr(i+1, 2).c_str(), 0, 16);
            url.replace(i, 3, 1, char(hex));
        }
    }
    return url;
}

//     static int hex_to_int(char c) {
//         if (c >= '0' && c <= '9') return c - '0';
//         if (c >= 'A' && c <= 'F') return c - 'A' + 10;
//         if (c >= 'a' && c <= 'f') return c - 'a' + 10;
//         return -1; // Invalid hex character
//     }
    
//     // Main URL decode function
//     static std::string url_decode(const std::string& url) {
//         std::string decoded;
//         decoded.reserve(url.length()); // Pre-allocate for efficiency
        
//         for (size_t i = 0; i < url.length(); ++i) {
//             if (url[i] == '%' && i + 2 < url.length()) {
//                 // Get the two hex digits after %
//                 int high = hex_to_int(url[i + 1]);
//                 int low = hex_to_int(url[i + 2]);
                
//                 // If both are valid hex digits
//                 if (high != -1 && low != -1) {
//                     // Convert to character and append
//                     char decoded_char = static_cast<char>((high * 16) + low);
//                     decoded += decoded_char;
//                     i += 2; // Skip the two hex digits
//                 } else {
//                     // Invalid encoding, keep the % as is
//                     decoded += url[i];
//                 }
//             } else if (url[i] == '+') {
//                 // Handle + as space (common in form data)
//                 decoded += ' ';
//             } else {
//                 // Regular character, copy as is
//                 decoded += url[i];
//             }
//         }
        
//         return decoded;
//     }
// };
// std::string GetHandler::readFile(const std::string& filePath) {
//     std::string extension;
//     if(!filePath.empty())
//     {
//         extension = getFileExtension(filePath);
//         std::map<std::string, std::string>::const_iterator it = contentTypes.find(extension);
//         if(it != contentTypes.end() && !extension.empty())
//         {
//             contentType = it->second;
//         }
//     }
//     std::ifstream file(filePath.c_str(),std::ios::binary);
//     if (!file) {
//         std::cout << "failed Read ///////\n";
//         return "";
//     }
//     size_t size = getFileSize(filePath);
//     std::cout << "@@@@@@@@@@@@@ |" << size << "| @@@@@@@@@@@@\n";
//     const size_t bufferSize = 1024; // Buffer size set to 8000 bytes
//     char buffer[bufferSize]; 
//     std::string content; // String to accumulate file content
//     ssize_t bytesSent = 0;
//     ssize_t TotalbytesSent = 0;
//     memset(&buffer, 0, bufferSize);
//     if(size > bufferSize * bufferSize)
//         generate_header(1);
//     else
//         generate_header(0);
//     while (file.read(buffer, bufferSize) || file.gcount() > 0) {
//         std::cout << "^^^^^^^^^^=> | " << file.gcount() << " |<=^^^^^^^^^\n";
//         std::stringstream size_header;
//         if(size > bufferSize * bufferSize)
//         {
//             size_header  <<  file.gcount() <<  "\r\n";
//             bytesSent = send(client_fd, size_header.str().c_str(), size_header.str().length(), 0);
//         }
//         bytesSent = send(client_fd, buffer, file.gcount(), 0);
//         if(size > bufferSize * bufferSize)
//             bytesSent = send(client_fd, "\r\n", 2, 0);
//         if (bytesSent < 0) {
//             std::cout << strerror(errno) << std::endl;
//             close(client_fd);
//             std::cout << "clooooooooooooooooooooooooooose(2) TotalbytesSent : " << TotalbytesSent << "| size : " << size << std::endl ;
//             std::cout << "size : |" << size << "| \n";
//             file.close();
//             return "";
//         }
//         TotalbytesSent = TotalbytesSent + bytesSent;
//     }
//     std::cout << "sennnnnnnnnnnnnnnnnnnnnnd complete TotalbytesSent : " << TotalbytesSent << "| size : " << size << std::endl ;
//     check_if = 1;
//     file.close();
//     return content;
// }