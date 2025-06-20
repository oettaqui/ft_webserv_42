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
    autoIndex = false;
    cgi_check = false;
    cgiHandler = NULL;
    cgi_error = false;
    use_final_res = false;
    cgi_flag = 0;
}

GetHandler::~GetHandler() {
    if(cgiHandler)
        delete cgiHandler;
}

void GetHandler::generate_header(int flag)
{
    if(check_put_header == 1)
        return ;
    std::stringstream header;
    if(flag == 0)
    {
        header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
                << "Content-Type: " << contentType << "\r\n"
                << "Content-Length: " << contentLength << "\r\n"
                << "Connection: close\r\n"
                << "\r\n";
    }
    else
    {
        header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Transfer-Encoding: chunked\r\n"
        << "\r\n";
    }
    final_res += header.str();
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
    return ""; 
}


void GetHandler::storeContentTypes(ParsRequest &request_data) {
    this->client_fd = request_data.getClientFd();
    contentTypes["html"] = "text/html";
    contentTypes["css"] = "text/css";
    contentTypes["xml"] = "text/xml";
    contentTypes["txt"] = "text/plain";
    contentTypes["md"] = "text/markdown";
    contentTypes["js"] = "text/javascript";
    contentTypes["json"] = "application/json";
    contentTypes["csv"] = "text/csv";
    contentTypes["gif"] = "image/gif";
    contentTypes["jpeg"] = "image/jpeg";
    contentTypes["jpg"] = "image/jpg";
    contentTypes["png"] = "image/png";
    contentTypes["webp"] = "image/webp";
    contentTypes["svg"] = "image/svg+xml";
    contentTypes["ico"] = "image/x-icon";
    contentTypes["bmp"] = "image/bmp";
    contentTypes["tiff"] = "image/tiff";
    contentTypes["mp3"] = "audio/mpeg";
    contentTypes["wav"] = "audio/wav";
    contentTypes["ogg"] = "audio/ogg";
    contentTypes["m4a"] = "audio/mp4";
    contentTypes["aac"] = "audio/aac";
    contentTypes["flac"] = "audio/flac";
    contentTypes["mid"] = "audio/midi";
    contentTypes["mp4"] = "video/mp4";
    contentTypes["webm"] = "video/webm";
    contentTypes["avi"] = "video/x-msvideo";
    contentTypes["mpg"] = "video/mpeg";
    contentTypes["mov"] = "video/quicktime";
    contentTypes["wmv"] = "video/x-ms-wmv";
    contentTypes["flv"] = "video/x-flv";
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
    contentTypes["woff"] = "font/woff";
    contentTypes["woff2"] = "font/woff2";
    contentTypes["ttf"] = "font/ttf";
    contentTypes["otf"] = "font/otf";
    contentTypes["eot"] = "application/vnd.ms-fontobject";
    contentTypes["php"] = "text";
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
    if(check_root(location_concerned.getRoot()))
        send_href = location_base + path.substr(location_concerned.getRoot().length() - 1,path.length());
    else if(path.empty() && location_concerned.getPath() == "/")
        send_href = base_path;
    else if(path.empty() && location_concerned.getPath() != "/")
        send_href = location_base;
    else
    {
        if(location_concerned.getPath() != "/")
            send_href = location_base + "/" + path.substr(location_concerned.getRoot().length(),path.length());
        else
            send_href = location_base + path.substr(location_concerned.getRoot().length(),path.length());
    }
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
        send_href = send_href.substr(1, send_href.length());
    for (size_t i = 0; i < items.size(); ++i) {
        std::string url = send_href + "/" + items[i];
        if(isDirectory(base_path + "/" + items[i]))
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + url_encode_question_marks(url) + "\">" + items[i] +" 📁</a>\n"
                    "            </li>\n";
        }
        else
        {
            html += "            <li class=\"link-item\">\n"
                    "                <a href=\"" + url_encode_question_marks(url) + "\">" + items[i] + " 📄</a>\n"
                    "            </li>\n";         
        }
    }

    html += "        </ul>\n"
            "        <div class=\"footer\">\n"
            "            bchokri | oettqui 42\n"
            "        </div>\n"
            "    </div>\n"
            "</body>\n"
            "</html>";
    contentLength = html.length();
    closedir(dir);
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
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

std::vector<std::string> GetHandler::check_root_location(std::string directoryPath)
{
    std::vector<std::string> fileList = listFiles(directoryPath);

    if (!fileList.empty()) {
        for (std::vector<std::string>::const_iterator it = fileList.begin(); it != fileList.end(); ++it) {
            std::string fullPath = directoryPath + "/" + *it;
        }
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
    final_res = "";
    storeContentTypes(request_data);
    path_location = this->split(url_decode(request_data.getPath()),'/');
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());;
    std::vector<std::string> find_vec = get_location_server();
    if(find_vec.size() != 0 )
    {
        location_base = find_vec.back();
        it_find_location_server = server_socket.getLocations().find(location_base);
        location_concerned =  it_find_location_server->second;
        ingore_element = split(find_vec.back(),'/').size();
        index_file = get_path_to_get();
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"GET") 
        != location_concerned.getMethods().end()))
        {
            statusCode = 405;
            status_message = "Method Not Allowed";
            std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
            if(itse != server_socket.getErrorPages().end())
            {
                std::string ex_error = getFileExtension(itse->second);
                if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                {
                    std::string return_value = readFile(itse->second,request_data);
                    if(check_if == 1)
                        return generateResponse(return_value, request_data);
                }
            }
            contentType = "text/html";
            contentLength = 54;
            generate_header(0);
            return generateResponse("<h1>405 the client doesn't have permission to GET</h1>", request_data);
        }
        else
        {
            std::string check_string = trim(index_file,'.');
            if(location_concerned.hasRedirect() == true)
            {
                const std::map<int, std::string>::const_iterator redirection = location_concerned.getRedirection().begin();
                content = createRedirectResponse(redirection->first,redirection->second);
                return generateResponse(content, request_data);
            }
            if(location_concerned.getAutoindex() == true && location_concerned.getPath() == check_string)
            {
                if(location_concerned.getIndex().size() != 0 )
                {
                    for(std::vector<std::string>::const_iterator it_index = location_concerned.getIndex().begin();it_index != location_concerned.getIndex().end();++it_index)
                    {
                        index_file = location_concerned.getRoot() + '/' + *it_index;
                        if (access(index_file.c_str(), R_OK) == 0) 
                        {
                            content = readFile(index_file,request_data);
                            break;
                        }
                            
                    }
                    if(content.empty() && check_if == 0)
                        content = readFile("./default/index.html",request_data);
                }
                else
                   content = readFile("./default/index.html",request_data);
                autoIndex = true;
            }
            else if(!isDirectory(index_file))
                content = readFile(index_file,request_data);
            else if(isDirectory(index_file))
            {
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file);
            }
        }
    }
    else if(find_vec.size() == 0)
    {
        location_base = "/";
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned =  it_find_location_server->second;
        index_file = get_path_to_get();
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"GET") 
        != location_concerned.getMethods().end()))
        {
            contentType = "text/html";
            statusCode = 405;
            std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
            if(itse != server_socket.getErrorPages().end())
            {
                std::string ex_error = getFileExtension(itse->second);
                if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                {
                    std::string return_value = readFile(itse->second,request_data);
                    if(check_if == 1)
                        return generateResponse(return_value, request_data);
                }
            }
            status_message = "Method Not Allowed";
            contentLength = 54;
            generate_header(0);
            return generateResponse("<h1>405 the client doesn't have permission to GET</h1>", request_data);
        }
        else
        {
            if(location_concerned.hasRedirect() == true)
            {
                const std::map<int, std::string>::const_iterator redirection = location_concerned.getRedirection().begin();
                content = createRedirectResponse(redirection->first,redirection->second);
                return generateResponse(content, request_data);
            }
            if(location_concerned.getAutoindex() == true && request_data.getPath() == "/")
            {
                for(std::vector<std::string>::const_iterator it_index = location_concerned.getIndex().begin();it_index != location_concerned.getIndex().end();++it_index)
                {
                    index_file = location_concerned.getRoot() + '/' + *it_index;
                    if (access(index_file.c_str(), R_OK) == 0) {
                        content = readFile(index_file,request_data);
                        break;
                    }
                        
                }
                if(content.empty() && check_if == 0)
                    content = readFile("./default/index.html",request_data);
                autoIndex = true;
            }
            else if(!isDirectory(index_file))
                content = readFile(index_file,request_data);
            else if(isDirectory(index_file))
            {
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file);
            }
        }
    }
    if (content.empty() && check_if == 0) {
        int not_found = 0;
        if(access(index_file.c_str(), F_OK) == 0)
        {
            statusCode = 403;
            status_message = "Forbidden";
            not_found = 1;
        }
        else
        {
            statusCode = 404;
            status_message = "Not Found";
        }
        std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
        if(itse != server_socket.getErrorPages().end())
        {
            std::string ex_error = getFileExtension(itse->second);
            if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
            {
                std::string return_value = readFile(itse->second,request_data);
                if(check_if == 1)
                    return generateResponse(return_value, request_data);
            }
        }
        contentType = "text/html";
        contentLength = 22;
        generate_header(0);
        if(not_found == 0)
            return generateResponse("<h1>404 Not Found</h1>", request_data);
        else
            return generateResponse("<h1>403 Forbidden</h1>", request_data);

    }
    return generateResponse(content, request_data);
}

std::string GetHandler::readFile(const std::string& filePath,ParsRequest &request_data) {
    (void)request_data;
    std::string extension;
    if(!filePath.empty()) {
        extension = getFileExtension(filePath);
        std::map<std::string, std::string>::const_iterator it = contentTypes.find(extension);
        if(it != contentTypes.end() && !extension.empty()) {
            contentType = it->second;
        }
    }
    if((extension == "php" || extension == "py" || extension == "pl") && (location_concerned.getCgi() && location_concerned.getCgiPass().size() > 0))
    {
        std::string response_cgi;
        if(!cgiHandler)
        {
            cgiHandler =  new CGI();
            std::string response;
            dataCGI data;
            data.method = request_data.getMethod();
            data.path = request_data.getPath();
            data.version = request_data.getVersion();
            data.file = "";
            data.contentType = contentType;
            data.contentLen = 0;
            data.scriptPath = filePath;
            data.queryString = request_data.getQuery();
            data.headers = request_data.getHeaders();
            if(autoIndex == true)
                data.autoIndex = "true";
            else
                data.autoIndex = "false";
            std::map<std::string, std::string> passCGI = location_concerned.getCgiPass();
            std::map<std::string, std::string>::iterator passCGIIT = passCGI.find( "." + extension);
            if (passCGIIT != passCGI.end())
                data.CorrectPassCGI = passCGIIT->second;
            else
            {
                contentType = "text/html";
                statusCode = 500;
                std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
                if(itse != server_socket.getErrorPages().end())
                {
                    std::string ex_error = getFileExtension(itse->second);
                    if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                    {
                        std::string return_value = readFile(itse->second,request_data);
                        if(check_if == 1)
                        {
                            cgi_error = true;
                            return generateResponse(return_value, request_data);
                        }
                    }
                }
                status_message = "Internal Server Error";
                contentLength = 34;
                generate_header(0);
                return ("<h1>500 Internal Server Error</h1>");
            }
            cgiHandler->setVarsEnv(data);
            response_cgi = cgiHandler->executeScript();
            if (cgiHandler->getStatusCGI() != 200 && response_cgi.empty()){
                contentType = "text/html";
                statusCode = cgiHandler->getStatusCGI();
                std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
                if(itse != server_socket.getErrorPages().end())
                {
                    std::string ex_error = getFileExtension(itse->second);
                    if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                    {
                        std::string return_value = readFile(itse->second,request_data);
                        if(check_if == 1)
                        {
                            cgi_error = true;
                            return generateResponse(return_value, request_data);
                        }
                    }
                }
                status_message = "Internal Server Error";
                contentLength = 34;
                generate_header(0);
                return ("<h1>500 Internal Server Error</h1>");
            }
            cgi_check = true;
            cgi_flag = cgiHandler->getCGIFlag();
            check_if = 1;
            return (response_cgi);
        }
        else
        {
            response_cgi = cgiHandler->executeScript();
            if (cgiHandler->getStatusCGI() != 200 && response_cgi.empty()){
                contentType = "text/html";
                statusCode = cgiHandler->getStatusCGI();
                std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
                if(itse != server_socket.getErrorPages().end())
                {
                    std::string ex_error = getFileExtension(itse->second);
                    if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                    {
                        std::string return_value = readFile(itse->second,request_data);
                        if(check_if == 1)
                        {
                            cgi_error = true;
                            return generateResponse(return_value, request_data);
                        }
                    }
                }
                status_message = "Internal Server Error";
                contentLength = 34;
                generate_header(0);
                return ("<h1>500 Internal Server Error</h1>");
            }
            cgi_flag = cgiHandler->getCGIFlag();
            return (response_cgi);
        }
    }
    if(!file.is_open())
    {
        file.open(filePath.c_str(), std::ios::binary);
        if (!file) {
            return "";
        }
    }
    
    if(size == 0)
    {
        size = getFileSize(filePath);
        contentLength = size;
    }
    const ssize_t threshold = 1024*1024;
    bool useChunked = (size > threshold);
    generate_header(useChunked ? 1 : 0);
    
    if (!useChunked) {
        return readSmallFile(file);
    } else {
        return readLargeFileChunked(file);
    }
}

std::string GetHandler::readSmallFile(std::ifstream& file) {
    std::string content;
    char buffer[BUFFER_SIZE_G];
    memset(buffer, 0, BUFFER_SIZE_G);
    file.read(buffer, BUFFER_SIZE_G);
    content.append(buffer, file.gcount());
    final_res += content;
    ssize_t bytesSent = send(client_fd, final_res.c_str(), final_res.length(), 0);
    if (bytesSent <= 0) {
        std::cerr << "Send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    totalBytesSent = totalBytesSent + file.gcount();
    check_if = 1;
    if(totalBytesSent >= size)
    {
        file.close();
        is_true_parse = true;
    }
    return "";
}

std::string GetHandler::readLargeFileChunked(std::ifstream& file) {
    char buffer[BUFFER_SIZE_G];
    int add_header = 0;
    size_t increment_value = 0;
    
    if (!isSocketAlive(client_fd)) {
        std::cerr << "Socket is dead, aborting transfer\n";
        file.close();
        return "";
    }
    
    memset(buffer, 0, BUFFER_SIZE_G);
    file.read(buffer, BUFFER_SIZE_G);
    size_t bytesRead = file.gcount();
    totalBytesSent += bytesRead;
    
    if (!final_res.empty())
        add_header = 1;
    
    std::stringstream chunkSize;
    chunkSize << std::hex << bytesRead << "\r\n";
    std::string chunkSizeStr = chunkSize.str();
    
    size_t totalChunkSize = chunkSizeStr.length() + bytesRead + 2;
    
    if (totalBytesSent >= size)
        totalChunkSize += 5;
    
    if (add_header == 1)
        totalChunkSize += final_res.length();

    std::vector<char> combinedBuffer(totalChunkSize);

    if (add_header == 1) {
        std::memcpy(combinedBuffer.data(), final_res.c_str(), final_res.length());
        increment_value += final_res.length();
    }

    std::memcpy(combinedBuffer.data() + increment_value, chunkSizeStr.c_str(), chunkSizeStr.length());
    increment_value += chunkSizeStr.length();

    std::memcpy(combinedBuffer.data() + increment_value, buffer, bytesRead);
    increment_value += bytesRead;

    std::memcpy(combinedBuffer.data() + increment_value, "\r\n", 2);
    increment_value += 2;

    if (totalBytesSent >= size) {
        std::memcpy(combinedBuffer.data() + increment_value, "0\r\n\r\n", 5);
        increment_value += 5;
    }
    ssize_t bytesSent = send(client_fd, combinedBuffer.data(), totalChunkSize,0);
    if (bytesSent <= 0) {
        std::cerr << "Chunk data send error: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    if (totalBytesSent >= size) {
        std::cout << "Chunked send complete. Total bytes sent: " << totalBytesSent << std::endl;
        file.close();
        is_true_parse = true;
    }
    
    check_if = 1;
    return "";
}
bool GetHandler::isSocketAlive(int sockfd) {
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    
    if (retval != 0 || error != 0) {
        return false;
    }
    
    return true;
}

bool GetHandler::get_is_true_parse() const
{
    return is_true_parse;
}

bool GetHandler::get_use_final_res() const
{
    return use_final_res;
}

bool GetHandler::getCgiCheck() const
{
    return cgi_check;
}

std::string GetHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    (void)request_data;
    if(cgi_error == true)
    {
        final_res.clear();
        cgi_error = false;
    }
    if(!content.empty() && cgi_check)
    {
        final_res += content;
        if(cgi_flag == 5)
            is_true_parse = true;
        use_final_res = true;
    }
    else if (!content.empty())
    {
        is_true_parse = true;
        final_res += content;
        use_final_res = true;
    }
    return final_res;
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

std::string GetHandler::url_encode_question_marks(std::string url) {
    for (std::string::size_type i = 0; i < url.length(); ++i) {
        if (url[i] == '?') {
            char hex_buffer[4];
            sprintf(hex_buffer, "%%%.2X", (unsigned char)('?'));
            url.replace(i, 1, hex_buffer);
            i += 2;
        }
    }
    return url;
}


std::string GetHandler::createRedirectResponse(int statusCode, const std::string& location) {
    std::stringstream response;
    bool isRedirect = (statusCode == 301 || statusCode == 302 || 
                      statusCode == 303 || statusCode == 307 || 
                      statusCode == 308);
    if (isRedirect) {
        response << "HTTP/1.1 " << statusCode << " ";
        switch(statusCode) {
            case 301: response << "Moved Permanently"; break;
            case 302: response << "Found"; break;
            case 303: response << "See Other"; break;
            case 307: response << "Temporary Redirect"; break;
            case 308: response << "Permanent Redirect"; break;
        }
        response << "\r\n";
        response << "Location: " << location << "\r\n";
        response << "Content-Length: 0\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        
    } 
    else {
        std::string body = location;
        response << "HTTP/1.1 " << statusCode << " ";
        response << "Unknown";
        response << "\r\n";
        response << "Content-Type: text/plain\r\n";
        response << "Content-Length: " << body.length() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
    }
    return response.str();
}
