#include "GetHandler.hpp"
#include "unistd.h"

GetHandler::GetHandler() 
{
}

void GetHandler::generate_header()
{
    std::stringstream header;
    header  << "HTTP/1.1 200 OK\r\n"
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

std::string GetHandler::generateAttractivePage(const std::vector<std::string>& items,const std::string &base_path,int flag) {
    if(flag == 1)
        generate_header();
    const std::string path = trim(base_path,'.');
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

    // Add each link as a styled item
    for (size_t i = 0; i < items.size(); ++i) {
        if(isDirectory(base_path + "/" + items[i]))
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + path + "/" + items[i] + "\">" + items[i] + " 📁</a>\n"
                    "            </li>\n";
        }
        else
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + path + "/" + items[i] + "\">" + items[i] + " 📄</a>\n"
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
        std::cerr << "Could not open directory: " << dirPath << std::endl;
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

std::string GetHandler::handleGetRequest(ParsRequest &request_data,ConfigParser &parser) {
    storeContentTypes(request_data);
    path_location = this->split(request_data.getPath(),'/');
    it_find_path_location = path_location.begin();
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::cout << "\n\nhandleGetRequest\n";
    // std::cout << "***host*** : " << request_data.hostMethod() << " ***port*** : " << request_data.portMethod() << std::endl;
    // if(server_socket.getPort() != 0)
    //     std::cout << "this is the server : ***|" <<  *server_socket.getServerNames().begin() << "|***" << std::endl;
    std::cout << "this is the path : ***|" <<  request_data.getPath() << "|***" << std::endl;
    if(path_location.size() != 0)
        location_base = '/' + *it_find_path_location;
    else
        location_base = "/";
    it_find_location_server = server_socket.getLocations().find(location_base);
    if(it_find_location_server != server_socket.getLocations().end())
    {
        std::cout << "++++++++++++++++++++++++++++++\n";
        location_concerned = it_find_location_server->second;
        size_t root_rs_sp = split(trim(location_concerned.getRoot(),'.'),'/').size() - 1;
        std::cout << "root_location : " << location_concerned.getRoot() << std::endl;
        fileList =  check_root_location(location_concerned.getRoot());
        if(location_concerned.getIndex().size() != 0 && location_concerned.getAutoindex() == true)
        {
            index_file = location_concerned.getRoot() + '/' + *location_concerned.getIndex().begin();
            if(std::find(fileList.begin(),fileList.end(),*location_concerned.getIndex().begin()) != fileList.end())
            {
                content = readFile(index_file);
            }
            else
                content = readFile("./default/index.html");
            
        }
        else if(it_find_path_location !=  path_location.end())
        {
            std::cout << "//////////////////////////////\n";
            it_find_path_location++;
            int check_else = 0;
            int check = 0;
            unsigned long count = root_rs_sp;
            for (std::vector<std::string>::const_iterator it = it_find_path_location; it != path_location.end(); ++it) {
                check = 1;
                if(std::find(fileList.begin(),fileList.end(),*it) != fileList.end())
                {
                    count++;
                    if(check_else == 0)
                        index_file = location_concerned.getRoot() + '/' + *it;
                    else
                        index_file = index_file + '/' + *it;
                    if(isDirectory(index_file))
                        check_else = 1;
                    else
                        check_else = 0;
                }
                if(check_else == 1)
                    fileList = check_root_location(index_file);
            }
            std::cout << "else : index_file : " << index_file << " | check_else : " << check_else << std::endl;
            if(check_else == 0 && count == path_location.size() - 1)
                content = readFile(index_file);
            else if(check_else == 1 && count == path_location.size() - 1)
            {
                // content = "<h1>is a folder you should list his content of this folder "+ index_file +"</h1>";
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file,1);
                std::cout << "++++++++ is a folder check_else +++++++++++\n" << index_file;
            }
            if(check == 0 && count == path_location.size() - 1)
            {
                index_file = location_concerned.getRoot();
                // content = "<h1>is a folder you should list his content of this folder "+ index_file +"</h1>";
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file,0);
                std::cout << "++++++++ is a folder check +++++++++++\n" << index_file;
            }
        }
        else if(it_find_path_location ==  path_location.end())
        {
            std::cout << "..............................\n";
            index_file = location_concerned.getRoot();
            // content = "<h1>is a folder you should list his content of this folder "+ index_file +"</h1>";
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,1);
            std::cout << "++++++++ is a folder check +++++++++++\n" << index_file;

        }
        std::cout << "\n\n";
    }
    else if(it_find_path_location !=  path_location.end())
    {
        std::cout << "---------------------------\n";
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned = it_find_location_server->second;
        size_t root_rs_sp = split(trim(location_concerned.getRoot(),'.'),'/').size() - 1;
        fileList =  check_root_location(location_concerned.getRoot());
        // it_find_path_location++;
        int check_else = 0;
        int check = 0;
        unsigned long count = root_rs_sp;
        for (std::vector<std::string>::const_iterator it = it_find_path_location; it != path_location.end(); ++it) {
            check = 1;
            if(std::find(fileList.begin(),fileList.end(),*it) != fileList.end())
            {
                count++;
                if(check_else == 0)
                    index_file = location_concerned.getRoot() + '/' + *it;
                else
                    index_file = index_file + '/' + *it;
                if(isDirectory(index_file))
                    check_else = 1;
                else
                    check_else = 0;
                
            }
            if(check_else == 1)
                fileList = check_root_location(index_file);
        }
        std::cout << "else : index_file : " << index_file << " | check_else : " << check_else << std::endl;
        std::cout << "else : count : " << count << " | path_location.size() - 1 : " << path_location.size() - 1 << std::endl;
        std::cout << "else : trim(location_concerned.getRoot(),'.') : " << trim(location_concerned.getRoot(),'.') << std::endl;
        std::cout << "root_rs_sp = " << root_rs_sp << std::endl;
        if(check_else == 0)
        {
            content = readFile(index_file);
        }
        else if(check_else == 1 && count == path_location.size() - 1)
        {
            // content = "<h1>is a folder you should list his content of this folder "+ index_file +"</h1>";
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,1);
            std::cout << "++++++++ is a folder check_else +++++++++++\n" << index_file;
        }
        if((check == 0 || 
            (trim(location_concerned.getRoot(),'.') == "/" + *path_location.begin() && path_location.size() == 1)) 
            && count == path_location.size() - 1)
        {
            index_file = location_concerned.getRoot();
            // content = "<h1>is a folder you should list his content of this folder "+ index_file +"</h1>";
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,0);
            std::cout << "++++++++ is a folder check +++++++++++\n" << index_file;
        }
    }
    else 
    {
        std::cout << "**********************************\n";
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned = it_find_location_server->second;
        std::vector<std::string> fileList =  check_root_location(location_concerned.getRoot());
        index_file = location_concerned.getRoot() + request_data.getPath();
        std::cout << "index_file : " << index_file << std::endl;
        if(std::find(fileList.begin(),fileList.end(),trim(request_data.getPath(),'/')) != fileList.end())
            content = readFile(index_file);
    }
    if (content.empty()) {
        return generateResponse("<h1>404 Not Found</h1>", request_data);
    }
    return generateResponse(content, request_data);
}
// get size compare size with 8000 if up read multiple time
//
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
    generate_header();
    std::ifstream file(filePath.c_str(),std::ios::binary);
    if (!file) {
        return "";
    }
    
    size_t size = getFileSize(filePath);
    std::cout << "@@@@@@@@@@@@@ |" << size << "| @@@@@@@@@@@@\n";
    const size_t bufferSize = 8000; // Buffer size set to 8000 bytes
    char buffer[bufferSize]; // Create a buffer to hold the data
    std::string content; // String to accumulate file content
    ssize_t bytesSent = 0;
    
    // Read and send the file in chunks
    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        std::cout << "^^^^^^^^^^=> | " << file.gcount() << " |<=^^^^^^^^^\n";
        
        // Send the buffer directly over the socket
        // if((size_t)file.gcount()  < bufferSize)
        // {
        //     content.append(buffer,file.gcount());
        //     break;
        // }
        bytesSent = send(client_fd, buffer, file.gcount(), 0);
        
        if (bytesSent < 0) {
            file.close();
            return "";
        }
    }
    
    file.close();

    return content;
}

std::string GetHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    std::stringstream response;
    (void)request_data;
    std::stringstream response1;
    response << content;
    // response1 << "badr\n";
    // send(request_data.getClientFd(), response1.str().c_str(), response1.str().length(), 0);
    return response.str();
}