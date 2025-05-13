#include "GetHandler.hpp"
#include "unistd.h"

// void GetHandler::ReadyToWork(const std::string& path)
// {
//     path_location = this->split(path,'/');
// }


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
        std::cout << "root_location : " << location_concerned.getRoot() << std::endl;
        fileList =  check_root_location(location_concerned.getRoot());
        if(location_concerned.getIndex().size() != 0 && location_concerned.getAutoindex() == true)
        {
            index_file = location_concerned.getRoot() + '/' + *location_concerned.getIndex().begin();
            if(std::find(fileList.begin(),fileList.end(),*location_concerned.getIndex().begin()) != fileList.end())
                content = readFile(index_file);
            
        }
        else if(it_find_path_location !=  path_location.end())
        {
            std::cout << "//////////////////////////////\n";
            it_find_path_location++;
            int check_else = 0;
            int check = 0;
            for (std::vector<std::string>::const_iterator it = it_find_path_location; it != path_location.end(); ++it) {
                check = 1;
                if(std::find(fileList.begin(),fileList.end(),*it) != fileList.end())
                {
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
            if(check_else == 0)
                content = readFile(index_file);
            else if(check_else == 1)
            {
                content = "<h1>is a folder you should list his content</h1>";
                std::cout << "++++++++ is a folder check_else +++++++++++\n" << index_file;
            }
            if(check == 0)
            {
                content = "<h1>is a folder you should list his content</h1>";
                index_file = location_concerned.getRoot();
                std::cout << "++++++++ is a folder check +++++++++++\n" << index_file;
            }
        }
        else if(it_find_path_location ==  path_location.end())
        {
            std::cout << "..............................\n";
            content = "<h1>is a folder you should list his content</h1>";
            index_file = location_concerned.getRoot();
            std::cout << "++++++++ is a folder check +++++++++++\n" << index_file;

        }
        std::cout << "\n\n";
    }
    else if(it_find_path_location !=  path_location.end())
    {
        std::cout << "---------------------------\n";
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned = it_find_location_server->second;
        fileList =  check_root_location(location_concerned.getRoot());
        int check_else = 0;
        int check = 0;
        for (std::vector<std::string>::const_iterator it = it_find_path_location; it != path_location.end(); ++it) {
            check = 1;
            if(std::find(fileList.begin(),fileList.end(),*it) != fileList.end())
            {
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
        if(check_else == 0)
            content = readFile(index_file);
        else if(check_else == 1)
        {
            content = "<h1>is a folder you should list his content</h1>";
            std::cout << "++++++++ is a folder check_else +++++++++++\n" << index_file;
        }
        if(check == 0 || 
            (trim(location_concerned.getRoot(),'.') == "/" + *path_location.begin() && path_location.size() == 1))
        {
            content = "<h1>is a folder you should list his content</h1>";
            index_file = location_concerned.getRoot();
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
        return generateResponse("<h1>404 Not Found</h1>", "text/html");
    }
    return generateResponse(content, contentType);
}

std::string GetHandler::readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (!file) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string GetHandler::generateResponse(const std::string& content, const std::string& contentType) {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << content;
    return response.str();
}