#ifndef CGIPOST_HPP
#define CGIPOST_HPP

#include <string>
#include <map>
#include <vector>
#include "../pars_request/ParsRequest.hpp"


class CGIPost{

private:
    std::string scriptPath;
    int contentLength;
    std::map<std::string, std::string> envVars;

    void setEnvironmentVariables(const ParsRequest& request);
    std::string executeScript();

public:
    CGIPost();
    ~CGIPost();

};


#endif




// #ifndef CGIPOST_HPP
// #define CGIPOST_HPP

// #include "../pars_request/ParsRequest.hpp"
// #include <string>
// #include <map>
// #include <vector>

// class CGIPost {
// private:
//     std::string scriptPath;
//     std::string tempFilePath;
//     std::map<std::string, std::string> envVars;
//     std::string postData;
//     int contentLength;
    
//     void setEnvironmentVariables(const ParsRequest& request);
//     void createTempFile(const std::string& body);
//     std::string executeScript();
//     void cleanup();

// public:
//     CGIPost();
//     ~CGIPost();
    
//     std::string handleRequest(const ParsRequest& request, const std::string& scriptPath);
// };

// #endif

// // CGIPost.cpp
// #include "CGIPost.hpp"
// #include <fstream>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <cstdlib>
// #include <iostream>
// #include <sstream>

// CGIPost::CGIPost() : contentLength(0) {
//     tempFilePath = "/tmp/cgi_post_" + std::to_string(getpid()) + "_" + std::to_string(time(nullptr));
// }

// CGIPost::~CGIPost() {
//     cleanup();
// }

// std::string CGIPost::handleRequest(const ParsRequest& request, const std::string& scriptPath) {
//     this->scriptPath = scriptPath;
    
//     // Step 1: Get POST body data
//     postData = request.getBody(); // Assume this method exists
//     contentLength = postData.length();
    
//     // Step 2: Set environment variables
//     setEnvironmentVariables(request);
    
//     // Step 3: Create temporary file with POST data
//     createTempFile(postData);
    
//     // Step 4: Execute the PHP script
//     std::string response = executeScript();
    
//     // Step 5: Cleanup
//     cleanup();
    
//     return response;
// }

// void CGIPost::setEnvironmentVariables(const ParsRequest& request) {
//     // Set standard CGI environment variables
//     envVars["REQUEST_METHOD"] = "POST";
//     envVars["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
//     envVars["CONTENT_LENGTH"] = std::to_string(contentLength);
//     envVars["SCRIPT_NAME"] = scriptPath;
//     envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
//     envVars["GATEWAY_INTERFACE"] = "CGI/1.1";
    
//     // Add other headers if needed
//     auto headers = request.getHeaders(); // Assume this method exists
//     for (const auto& header : headers) {
//         std::string envName = "HTTP_" + header.first;
//         // Convert to uppercase and replace - with _
//         for (char& c : envName) {
//             if (c == '-') c = '_';
//             else c = toupper(c);
//         }
//         envVars[envName] = header.second;
//     }
// }

// void CGIPost::createTempFile(const std::string& body) {
//     std::ofstream tempFile(tempFilePath);
//     if (!tempFile.is_open()) {
//         throw std::runtime_error("Failed to create temp file");
//     }
//     tempFile << body;
//     tempFile.close();
// }

// std::string CGIPost::executeScript() {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }
    
//     pid_t pid = fork();
//     if (pid == -1) {
//         throw std::runtime_error("Failed to fork");
//     }
    
//     if (pid == 0) {
//         // Child process
        
//         // Set environment variables
//         for (const auto& env : envVars) {
//             setenv(env.first.c_str(), env.second.c_str(), 1);
//         }
        
//         // Redirect stdin to temp file
//         freopen(tempFilePath.c_str(), "r", stdin);
        
//         // Redirect stdout to pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[0]);
//         close(pipefd[1]);
        
//         // Execute PHP script
//         execl("/usr/bin/php", "php", scriptPath.c_str(), (char*)NULL);
        
//         // If we reach here, exec failed
//         exit(1);
//     } else {
//         // Parent process
//         close(pipefd[1]);
        
//         // Read output from child
//         std::string output;
//         char buffer[4096];
//         ssize_t bytesRead;
        
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             output.append(buffer, bytesRead);
//         }
        
//         close(pipefd[0]);
        
//         // Wait for child to finish
//         int status;
//         waitpid(pid, &status, 0);
        
//         return output;
//     }
// }

// void CGIPost::cleanup() {
//     // Remove temporary file
//     if (!tempFilePath.empty()) {
//         unlink(tempFilePath.c_str());
//     }
// }

// // Usage in your main code:
// /*
// if (postHandler->isRequestComplete()) {
//     if (postHandler->getCGIState()) {
//         CGIPost* cgiHandler = new CGIPost();
        
//         // Get the script path (submit.php in your case)
//         std::string scriptPath = "./submit.php";
        
//         // Execute CGI and get response
//         std::string response = cgiHandler->handleRequest(*parsedRequest, scriptPath);
        
//         // Send response back to client
//         // ... your response handling code ...
        
//         delete cgiHandler;
//     }
//     is_Complet = true;
// }
// */