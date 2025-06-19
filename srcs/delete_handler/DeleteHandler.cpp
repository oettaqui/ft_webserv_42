#include "DeleteHandler.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>



DeleteHandler::DeleteHandler() {
    check_put_header = 0;
    ingore_element = 0;
    check_if = 0;
    existent_folder = 0;
    statusCode = 200;
    status_message = "OK";
    permitions_folder = 0;
    totalBytesSent = 0;
    size = 0;
    is_true_parse = false;
    contentLength = 0;
    final_res = "";
    use_final_res = false;
}

DeleteHandler::~DeleteHandler() {
}

bool DeleteHandler::get_is_true_parse() const
{
    return is_true_parse;
}


void DeleteHandler::generate_header()
{
    if(check_put_header == 1)
        return ;
    std::stringstream header;
    header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
            << "Content-Type: " << contentType << "\r\n"
            << "Content-Length: " << contentLength << "\r\n"
            << "Connection: close\r\n"
            << "\r\n";
    final_res += header.str();
    check_put_header = 1;
} 

void DeleteHandler::storeContentTypes(ParsRequest &request_data) {
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

size_t DeleteHandler::getFileSize(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string DeleteHandler::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return "";
}

std::string DeleteHandler::generateAttractivePage(const std::string &base_path) {
    std::string html;
    if(deleteDirectory(base_path) == true)
    {
        html = "<!DOCTYPE html>\r\n"
                            "<html>\r\n"
                            "<head>\r\n"
                            "    <title>Successfully deleted directory</title>\r\n"
                            "</head>\r\n"
                            "<body>\r\n"
                            "    <h1>Successfully deleted directory</h1>\r\n"
                            "</body>\r\n"
                            "</html>\r\n";
        generate_header();
    }
    else
    {
        statusCode = 404;
        status_message = "Not Found";
        check_if = 0;
        return "";
    }
    check_if = 1;
    return html;
}
std::vector<std::string> DeleteHandler::split(const std::string& str, char delim) const {
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

std::vector<std::string> DeleteHandler::listFiles(const std::string& dirPath) {
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
        existent_folder = 1;
    return files;
}

bool DeleteHandler::isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

std::vector<std::string> DeleteHandler::check_root_location(std::string directoryPath)
{
    std::vector<std::string> fileList = listFiles(directoryPath);

    if (!fileList.empty()) {
        for (std::vector<std::string>::const_iterator it = fileList.begin(); it != fileList.end(); ++it) {
            std::string fullPath = directoryPath + "/" + *it;
        }
    } 
    else {
        std::cerr << "--> No files found in directory '" << directoryPath << "'" << std::endl;
    }
    return (fileList);
}

std::string DeleteHandler::trim(const std::string& str, char ch) {
    std::string::size_type start = str.find_first_not_of(ch);
    std::string::size_type end = str.find_last_not_of(ch);

    if (start == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::string DeleteHandler::get_path_to_get()
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


std::vector<std::string> DeleteHandler::get_location_server() const
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



std::string DeleteHandler::handleDeleteRequest(ParsRequest &request_data,ConfigParser &parser) {
    final_res.clear();
    storeContentTypes(request_data);
    path_location = this->split(url_decode(request_data.getPath()),'/');
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::vector<std::string> find_vec = get_location_server();
    if(find_vec.size() != 0 )
    {
        location_base = find_vec.back();
        it_find_location_server = server_socket.getLocations().find(location_base);
        location_concerned =  it_find_location_server->second;
        ingore_element = split(find_vec.back(),'/').size();
        index_file = get_path_to_get();
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"DELETE") 
        != location_concerned.getMethods().end()))
        {
            statusCode = 405;
            status_message = "Method Not Allowed";
            contentLength = 57;
            std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
            if(itse != server_socket.getErrorPages().end())
            {
                std::string ex_error = getFileExtension(itse->second);
                if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                {
                    std::string return_value = readFile_v2(itse->second);
                    if(check_if == 1)
                        return generateResponse(return_value, request_data);
                }
            }
            generate_header();
            return generateResponse("<h1>405 the client doesn't have permission to DELETE</h1>", request_data);
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
                for(std::vector<std::string>::const_iterator it_index = location_concerned.getIndex().begin();it_index != location_concerned.getIndex().end();++it_index)
                {
                    index_file = location_concerned.getRoot() + '/' + *it_index;
                    if (access(index_file.c_str(), R_OK) == 0)
                    {
                        content = readFile(index_file);
                        break;
                    }
                        
                }
                if(content.empty() && check_if == 0)
                    content = readFile("./default/index.html");
            }
            else if(!isDirectory(index_file))
                content = readFile(index_file);
            else if(isDirectory(index_file))
                content = generateAttractivePage(index_file);
        }
    }
    else if(find_vec.size() == 0)
    {
        location_base = "/";
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned =  it_find_location_server->second;
        index_file = get_path_to_get();
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"DELETE") 
        != location_concerned.getMethods().end()))
        {
            status_message = "Method Not Allowed";
            statusCode = 405;
            contentLength = 57; 
            std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
            if(itse != server_socket.getErrorPages().end())
            {
                std::string ex_error = getFileExtension(itse->second);
                if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
                {
                    std::string return_value = readFile_v2(itse->second);
                    if(check_if == 1)
                        return generateResponse(return_value, request_data);
                }
            }
            generate_header();
            return generateResponse("<h1>405 the client doesn't have permission to DELETE</h1>", request_data);
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
                    if (access(index_file.c_str(), R_OK) == 0)
                    {
                        content = readFile(index_file);
                        break;
                    }
                        
                }
                if(content.empty() && check_if == 0)
                    content = readFile("./default/index.html");
            }
            else if(!isDirectory(index_file))
                content = readFile(index_file);
            else if(isDirectory(index_file))
                content = generateAttractivePage(index_file);
        }
    }
    if (content.empty() && check_if == 0) {
        statusCode = 404;
        status_message = "Not Found";
        std::map<int, std::string>::const_iterator itse = server_socket.getErrorPages().find(statusCode);
        if(itse != server_socket.getErrorPages().end())
        {
            std::string ex_error = getFileExtension(itse->second);
            if(ex_error != "php" && ex_error != "py" && ex_error != "pl")
            {
                std::string return_value = readFile_v2(itse->second);
                if(check_if == 1)
                    return generateResponse(return_value, request_data);
            }
        }
        contentLength = 22;
        generate_header();
        return generateResponse("<h1>404 Not Found</h1>", request_data);
    }
    return generateResponse(content, request_data);
}


std::string DeleteHandler::readSmallFile(std::ifstream& file) {
    std::string content;
    char buffer[BUFFER_SIZE_G];
    memset(buffer, 0, BUFFER_SIZE_G);
    file.read(buffer, BUFFER_SIZE_G);
    content.append(buffer, file.gcount());
    final_res += content;
    ssize_t bytesSent = send(client_fd, final_res.c_str(), final_res.length(), 0);
    if (bytesSent <= 0) {
        std::cout << "Send error : " << strerror(errno) << std::endl;
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

std::string DeleteHandler::readFile_v2(const std::string& filePath) {
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
    generate_header();
    return readSmallFile(file);
}

std::string DeleteHandler::readFile(const std::string& filePath) {
    std::string content;
    if(DeleteHandler::deleteFile(filePath) == true)
    {
        content = "<!DOCTYPE html>\r\n"
                            "<html>\r\n"
                            "<head>\r\n"
                            "    <title>Successfully deleted file</title>\r\n"
                            "</head>\r\n"
                            "<body>\r\n"
                            "    <h1>Successfully deleted file</h1>\r\n"
                            "</body>\r\n"
                            "</html>\r\n";
        generate_header();
    }
    else
    {
        check_if = 0;
        return "";
    }
    check_if = 1;
    return content;
}

std::string DeleteHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    (void)request_data;
    if (!content.empty())
    {
        is_true_parse = true;
        final_res += content;
        use_final_res = true;
    }
    return final_res;
}

bool DeleteHandler::resourceExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool DeleteHandler::deleteFile(const std::string& path) {
    if (remove(path.c_str()) != 0) {
        std::cerr << "Error deleting file: " << path << std::endl;
        return false;
    }
    std::cout << "Successfully deleted file: " << path << std::endl;
    return true;
}

bool DeleteHandler::deleteDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Error opening directory: " << path << std::endl;
        if(resourceExists(path))
            permitions_folder = 1;
        return false;
    }
    
    struct dirent* entry;
    bool success = true;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }
        
        std::string full_path = path + "/" + name;
        struct stat path_stat;
        stat(full_path.c_str(), &path_stat);
        
        if (S_ISDIR(path_stat.st_mode)) {
            if (!deleteDirectory(full_path)) {
                success = false;
            }
        } else {
            if (!deleteFile(full_path)) {
                success = false;
            }
        }
    }
    
    closedir(dir);
    if (success && rmdir(path.c_str()) != 0) {
        std::cerr << "Error deleting directory: " << path << std::endl;
        success = false;
    }
    
    if (success) {
        std::cout << "Successfully deleted directory: " << path << std::endl;
    }
    
    return success;
}

std::string DeleteHandler::url_decode(std::string url) {
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%') {
            int hex = strtol(url.substr(i+1, 2).c_str(), 0, 16);
            url.replace(i, 3, 1, char(hex));
        }
    }
    return url;
}


bool DeleteHandler::get_use_final_res() const
{
    return use_final_res;
}

std::string DeleteHandler::createRedirectResponse(int statusCode, const std::string& location) {
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


