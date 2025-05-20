#include "DeleteHandler.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>



DeleteHandler::DeleteHandler() {
    check_put_header = 0;
    check_if = 0;
    existent_folder = 0;
    statusCode = 200;
    status_message = "OK";
}

DeleteHandler::~DeleteHandler() {
    // Destructor
}

void DeleteHandler::generate_header()
{
    std::cout << "=======================>\n";
    std::stringstream header;
    header  << "HTTP/1.1 "<< statusCode << " " <<  status_message << "\r\n"
            << "Content-Type: " << contentType << "\r\n"
            << "Connection: close\r\n"
            << "\r\n";
    send(client_fd, header.str().c_str(), header.str().length(), 0);
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

std::string DeleteHandler::generateAttractivePage(const std::vector<std::string>& items,const std::string &base_path,int flag) {
    if(flag == 1 && check_put_header == 0 && existent_folder != 1)
    {
        std::cout << "header put 2\n";
        generate_header();
    }
    (void)items;
    const std::string path = trim(base_path,'.');
    std::string html = "<!DOCTYPE html>\r\n"
                        "<html>\r\n"
                        "<head>\r\n"
                        "    <title>Remove Folder</title>\r\n"
                        "</head>\r\n"
                        "<body>\r\n"
                        "    <h1>Remove Folder</h1>\r\n"
                        "</body>\r\n"
                        "</html>\r\n";

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

std::string DeleteHandler::trim(const std::string& str, char ch) {
    std::string::size_type start = str.find_first_not_of(ch);
    std::string::size_type end = str.find_last_not_of(ch);

    if (start == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::string DeleteHandler::generateResponse(const std::string& content,ParsRequest &request_data) {
    std::stringstream response;
    (void)request_data;
    if((check_put_header == 1 && check_if != 1) || existent_folder == 1)
        generate_header();
    response << content;
    return response.str();
}

std::string DeleteHandler::readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str(),std::ios::binary);
    if (!file || check_put_header == 1) {
        return "";
    }
    if(check_put_header == 0)
        generate_header();
    std::cout << "====== >> file path <<  ======> " << index_file << std::endl;
    std::string content = "<!DOCTYPE html>\r\n"
                        "<html>\r\n"
                        "<head>\r\n"
                        "    <title>Remove file</title>\r\n"
                        "</head>\r\n"
                        "<body>\r\n"
                        "    <h1>Remove file</h1>\r\n"
                        "</body>\r\n"
                        "</html>\r\n";
    return content;
}

std::string DeleteHandler::handleDeleteRequest(ParsRequest &request_data,ConfigParser &parser) {
    this->client_fd = request_data.getClientFd();
    path_location = this->split(request_data.getPath(),'/');
    it_find_path_location = path_location.begin();
    contentType = "text/html";
    server_socket = parser.getServer(request_data.hostMethod(),request_data.portMethod());
    std::cout << "\n\nhandleDeleteRequest\n";
    if(path_location.size() != 0)
        location_base = '/' + *it_find_path_location;
    else
        location_base = "/";
    it_find_location_server = server_socket.getLocations().find(location_base);
    if(it_find_location_server != server_socket.getLocations().end())
    {
        location_concerned = it_find_location_server->second;
        size_t root_rs_sp = split(trim(location_concerned.getRoot(),'.'),'/').size() - 1;
        std::vector<std::string> root_split_rs = split(trim(location_concerned.getRoot(),'.'),'/');
        fileList =  check_root_location(location_concerned.getRoot());
        if(location_concerned.getIndex().size() != 0 && location_concerned.getAutoindex() == true)
        {
            index_file = location_concerned.getRoot() + '/' + *location_concerned.getIndex().begin();
            if(std::find(fileList.begin(),fileList.end(),*location_concerned.getIndex().begin()) != fileList.end())
                content = readFile(index_file);
            else
                content = readFile("./default/index.html");
            
        }
        else if(it_find_path_location !=  path_location.end())
        {
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
            if(check_else == 0 && count == path_location.size() - 1)
                content = readFile(index_file);
            else if(check_else == 1 && count == path_location.size() - 1)
            {
                std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file,1);
            }
            if(check == 0 && count == path_location.size() - 1)
            {
                index_file = location_concerned.getRoot();
                std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file,1);
            }
            else if((count != std::string::npos && root_rs_sp != std::string::npos) 
            && (count == root_rs_sp && existent_folder != 1))
            {
                if(root_split_rs.back() == path_location.back() || path_location.size() == 1)
                {
                    index_file = location_concerned.getRoot();
                    std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
                    fileList = check_root_location(index_file);
                    content = generateAttractivePage(fileList,index_file,1);
                }
            }
        }
        else if(it_find_path_location ==  path_location.end())
        {
            index_file = location_concerned.getRoot();
            std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,1);
        }
        std::cout << "\n\n";
    }
    else if(it_find_path_location !=  path_location.end())
    {
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned = it_find_location_server->second;
        size_t root_rs_sp = split(trim(location_concerned.getRoot(),'.'),'/').size() - 1;
        std::vector<std::string> root_split_rs = split(trim(location_concerned.getRoot(),'.'),'/');
        fileList =  check_root_location(location_concerned.getRoot());
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
        if(check_else == 0)
            content = readFile(index_file);
        else if(check_else == 1 && count == path_location.size() - 1)
        {
            std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,1);
        }
        if((check == 0 || 
            (trim(location_concerned.getRoot(),'.') == "/" + *path_location.begin() && path_location.size() == root_rs_sp + 1)) 
            && count == path_location.size() - 1) 
        {
            index_file = location_concerned.getRoot();
            std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
            fileList = check_root_location(index_file);
            content = generateAttractivePage(fileList,index_file,0);
            check_put_header = 1;
        }
        else if((count != std::string::npos && root_rs_sp != std::string::npos) && (count == root_rs_sp && existent_folder != 1))
        {
            if(root_split_rs.back() == path_location.back() || (path_location.size() == 1 && path_location.back() == "/"))
            {
                index_file = location_concerned.getRoot();
                std::cout << "====== >> folder path <<  ======> " << index_file << std::endl;
                fileList = check_root_location(index_file);
                content = generateAttractivePage(fileList,index_file,1); 
            }
        }
    }
    else 
    {
        it_find_location_server = server_socket.getLocations().find("/");
        location_concerned = it_find_location_server->second;
        std::vector<std::string> fileList =  check_root_location(location_concerned.getRoot());
        index_file = location_concerned.getRoot() + request_data.getPath();
        if(std::find(fileList.begin(),fileList.end(),trim(request_data.getPath(),'/')) != fileList.end())
            content = readFile(index_file);
    }
    if ((content.empty() && check_if != 1) || existent_folder == 1) {
        check_put_header = 1;
        statusCode = 404;
        status_message = "Not Found";
        return generateResponse("<h1>404 Not Found</h1>", request_data);
    }
    return generateResponse(content, request_data);
}


// bool DeleteHandler::resourceExists(const std::string& path) {
//     struct stat buffer;
//     return (stat(path.c_str(), &buffer) == 0);
// }

// bool DeleteHandler::deleteFile(const std::string& path) {
//     // Attempt to remove the file
//     if (remove(path.c_str()) != 0) {
//         std::cerr << "Error deleting file: " << path << std::endl;
//         return false;
//     }
//     std::cout << "Successfully deleted file: " << path << std::endl;
//     return true;
// }

// bool DeleteHandler::deleteDirectory(const std::string& path) {
//     DIR* dir = opendir(path.c_str());
//     if (!dir) {
//         std::cerr << "Error opening directory: " << path << std::endl;
//         return false;
//     }
    
//     struct dirent* entry;
//     bool success = true;
    
//     // First, recursively delete all contents
//     while ((entry = readdir(dir)) != NULL) {
//         std::string name = entry->d_name;
        
//         // Skip "." and ".." directories
//         if (name == "." || name == "..") {
//             continue;
//         }
        
//         std::string full_path = path + "/" + name;
//         struct stat path_stat;
//         stat(full_path.c_str(), &path_stat);
        
//         if (S_ISDIR(path_stat.st_mode)) {
//             // Recursive call for subdirectories
//             if (!deleteDirectory(full_path)) {
//                 success = false;
//             }
//         } else {
//             // Delete file
//             if (!deleteFile(full_path)) {
//                 success = false;
//             }
//         }
//     }
    
//     closedir(dir);
    
//     // Then delete the empty directory itself
//     if (success && rmdir(path.c_str()) != 0) {
//         std::cerr << "Error deleting directory: " << path << std::endl;
//         success = false;
//     }
    
//     if (success) {
//         std::cout << "Successfully deleted directory: " << path << std::endl;
//     }
    
//     return success;
// }

// std::string DeleteHandler::createResponse(int statusCode, const std::string& message) {
//     std::string response = "HTTP/1.1 " + std::to_string(statusCode) + " " + message + "\r\n";
//     response += "Content-Type: text/plain\r\n";
//     response += "Connection: close\r\n";
//     response += "\r\n";
    
//     if (statusCode == 200) {
//         response += "Resource successfully deleted.";
//     } else if (statusCode == 404) {
//         response += "The requested resource could not be found.";
//     } else if (statusCode == 500) {
//         response += "Internal server error occurred while attempting to delete the resource.";
//     }
    
//     return response;
// }







