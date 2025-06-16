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
    // Destructor
}

bool DeleteHandler::get_is_true_parse() const
{
    return is_true_parse;
}


void DeleteHandler::generate_header()
{
    if(check_put_header == 1)
        return ;
    std::cout << "=======================>\n";
    std::stringstream header;
    header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
            << "Content-Type: " << contentType << "\r\n"
            << "Content-Length: " << contentLength << "\r\n"
            << "Connection: close\r\n"
            << "\r\n";
    final_res += header.str();
    check_put_header = 1;
    // send(client_fd, header.str().c_str(), header.str().length(), 0);
} 

void DeleteHandler::storeContentTypes(ParsRequest &request_data) {
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
    std::cout << "====== >> base_path folder path <<  ======> " << base_path << std::endl;
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
        std::cout << "Files in directory '" << directoryPath << "':" << std::endl;
        for (std::vector<std::string>::const_iterator it = fileList.begin(); it != fileList.end(); ++it) {
            // std::cout << *it << std::endl;
            std::string fullPath = directoryPath + "/" + *it;
            // if (isDirectory(fullPath)) {
            //     std::cout << "  (Directory)" << std::endl;
            // } else {
            //     std::cout << "  (File)" << std::endl;
            // }
        }
    } 
    else {
        std::cout << "--> No files found in directory '" << directoryPath << "'" << std::endl;
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
    std::cout << "\n\nhandleDeleteRequest\n";
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
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"DELETE") 
        != location_concerned.getMethods().end()))
        {
            statusCode = 403;
            status_message = "Forbidden";
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
            return generateResponse("<h1>403 the client doesn't have permission to DELETE</h1>", request_data);
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
                content = generateAttractivePage(index_file);
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
        if(!(std::find(location_concerned.getMethods().begin(),location_concerned.getMethods().end(),"DELETE") 
        != location_concerned.getMethods().end()))
        {
            status_message = "Forbidden";
            statusCode = 403;
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
            return generateResponse("<h1>403 the client doesn't have permission to DELETE</h1>", request_data);
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
                content = generateAttractivePage(index_file);
            }
            else
            {
                std::cout << "haaaaaaaaaaaaaaaaa3\n";
                content = "<h1>test 1</h1>";
                generate_header();
            }
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
        std::cout << "aaaaaaaaaaaaaaaaaaaaaaaa\n";
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
    // std::cout << "-**********************************-\n"; 
    // std::cout << file.gcount() << std::endl;
    // std::cout << "**********************************\n";
    ssize_t bytesSent = send(client_fd, final_res.c_str(), final_res.length(), 0);
    if (bytesSent <= 0) {
        std::cout << "Send error12: " << strerror(errno) << std::endl;
        close(client_fd);
        file.close();
        return "";
    }
    totalBytesSent = totalBytesSent + file.gcount();
    check_if = 1;
    if(totalBytesSent >= size)
    {
        // std::cout << "++++++++++++++++++++++++++++++333333333\n";
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
    generate_header();
    return readSmallFile(file);
}

std::string DeleteHandler::readFile(const std::string& filePath) {
    std::string content;
    std::cout << "====== >> file path <<  ======> " << index_file << std::endl;
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
    std::cout << "check_put_header : " << check_put_header << std::endl;
    std::cout << "check_if : " << check_if << std::endl;
    std::cout << "existent_folder : " << existent_folder << std::endl;
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
    // Attempt to remove the file
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
    
    // First, recursively delete all contents
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        
        // Skip "." and ".." directories
        if (name == "." || name == "..") {
            continue;
        }
        
        std::string full_path = path + "/" + name;
        struct stat path_stat;
        stat(full_path.c_str(), &path_stat);
        
        if (S_ISDIR(path_stat.st_mode)) {
            // Recursive call for subdirectories
            if (!deleteDirectory(full_path)) {
                success = false;
            }
        } else {
            // Delete file
            if (!deleteFile(full_path)) {
                success = false;
            }
        }
    }
    
    closedir(dir);
    
    // Then delete the empty directory itself
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




