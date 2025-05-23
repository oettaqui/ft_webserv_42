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
}

void GetHandler::generate_header()
{
    std::cout << "=======================>\n";
    std::stringstream header;
    header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
            << "Content-Type: " << contentType << "\r\n"
            << "Connection: close\r\n"
            << "\r\n";
    send(client_fd, header.str().c_str(), header.str().length(), 0);
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

std::string GetHandler::generateAttractivePage(const std::vector<std::string>& items,const std::string &base_path) {
    generate_header();
    const std::string path = trim(base_path,'.');
    std::string send_href = location_base + path.substr(location_concerned.getRoot().length() - 1,path.length());
    std::cout << "======================| location_base |============ :: " << location_base << std::endl;
    std::cout << "======================| path |============ :: " << path << std::endl;
    std::cout << "======================| first send_href |============ :: " << send_href << std::endl;
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

    if(location_base == "/")
        send_href = send_href.substr(1, send_href.length());
    for (size_t i = 0; i < items.size(); ++i) {
        if(isDirectory(base_path + "/" + items[i]))
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + send_href + "/" + items[i] + "\">" +  items[i] +" 📁</a>\n"
                    "            </li>\n";
        }
        else
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + send_href + "/" + items[i] + "\">"  + items[i] + " 📄</a>\n"
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
    path_location = this->split(request_data.getPath(),'/');
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::cout << "\n\nhandleGetRequest\n";
    std::cout << "this is the path : ***|" <<  request_data.getPath() << "|***" << std::endl;
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
            generate_header();
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
            else
            {
                content = "<h1>test 1</h1>";
                generate_header();
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
            std::cout << "imposiiiii \n";
            statusCode = 403;
            status_message = "Forbidden";
            generate_header();
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
                content = readFile(index_file);
            }
            else if(isDirectory(index_file))
            {
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file);
            }
            else
            {
                content = "<h1>test 1</h1>";
                generate_header();
            }
        }
    }
    if (content.empty() && check_if == 0) {
        check_put_header = 1;
        statusCode = 404;
        status_message = "Not Found";
        std::cout << "aaaaaaaaaaaaaaaaaaaaaaaa\n";
        generate_header();
        return generateResponse("<h1>404 Not Found</h1>", request_data);
    }
    return generateResponse(content, request_data);
}

std::string GetHandler::readFile(const std::string& filePath) {
    // char str[8000] ={0};
    std::string extension;
    if(!filePath.empty())
    {
        extension = getFileExtension(filePath);
        std::map<std::string, std::string>::const_iterator it = contentTypes.find(extension);
        if(it != contentTypes.end() && !extension.empty())
        {
            contentType = it->second;
        }
    }
    std::ifstream file(filePath.c_str(),std::ios::binary);
    if (!file) {
        std::cout << "failed Read ///////\n";
        return "";
    }
    // if(check_put_header == 0)
    // {
    //     std::cout << "header put\n";
    //     generate_header();
    // }
    generate_header();
    size_t size = getFileSize(filePath);
    std::cout << "@@@@@@@@@@@@@ |" << size << "| @@@@@@@@@@@@\n";
    const size_t bufferSize = 8000; // Buffer size set to 8000 bytes
    char buffer[bufferSize]; 
    std::string content; // String to accumulate file content
    ssize_t bytesSent = 0;
    ssize_t TotalbytesSent = 0;
    memset(&buffer, 0, bufferSize);
    // Read and send the file in chunks
    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        std::cout << "^^^^^^^^^^=> | " << file.gcount() << " |<=^^^^^^^^^\n";
        bytesSent = send(client_fd, buffer, file.gcount(), 0);
        if (bytesSent < 0 || bytesSent < file.gcount()) {
            close(client_fd);
            std::cout << "clooooooooooooooooooooooooooose TotalbytesSent : " << TotalbytesSent << "| size : " << size << std::endl ;
            std::cout << "size : |" << size << "| \n";
            file.close();
            std::cout << strerror(errno) << std::endl;
            return "";
        }
        TotalbytesSent = TotalbytesSent + bytesSent;
    }
    std::cout << "sennnnnnnnnnnnnnnnnnnnnnd complete TotalbytesSent : " << TotalbytesSent << "| size : " << size << std::endl ;
    check_if = 1;
    file.close();
    return content;
}

std::string GetHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    std::stringstream response;
    (void)request_data;
    std::cout << "check_put_header : " << check_put_header << std::endl;
    std::cout << "check_if : " << check_if << std::endl;
    std::cout << "existent_folder : " << existent_folder << std::endl;
    response << content;
    return response.str();
}
