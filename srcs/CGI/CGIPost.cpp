#include "./CGIPost.hpp"

CGIPost::CGIPost(){
    // std::cout << "constractor " << std::endl;
}

CGIPost::~CGIPost(){
    // std::cout << "destractor " << std::endl;
    
}

void CGIPost::setVarsEnv(dataCGI& data) {

    envVars["REQUEST_METHOD"] = data.method;
    envVars["SCRIPT_NAME"] = data.scriptPath;
    envVars["SCRIPT_FILENAME"] = data.scriptPath;
    envVars["PATH_INFO"] = data.path;
    envVars["SERVER_PROTOCOL"] = data.version;
    envVars["CONTENT_LENGTH"] = toString(data.contentLen);
    envVars["CONTENT_TYPE"] = data.contentType;
    envVars["UPLOAD_FILE"] = data.file;
    envVars["AUTO_INDEX"] = data.autoIndex;
    envVars["PASS_CGI"] = data.CorrectPassCGI;
    envVars["QUERY_STRING"] = data.queryString;
    envVars["REDIRECT_STATUS"] = "200";

    this->file = data.file;
    this->scriptPath = data.scriptPath;
    this->passCgi = data.CorrectPassCGI;
    this->autoIndex = data.autoIndex;


}


std::string CGIPost::toString(size_t n) {
        std::ostringstream oss;
        oss << n;
        return oss.str();
}

char** CGIPost::buildEnvp(std::map<std::string, std::string>& env) {
    char** envp = new char*[env.size() + 1];
    std::map<std::string, std::string>::iterator it = env.begin();
    size_t i = 0;
    for (; it != env.end(); ++it, ++i) {
        std::string entry = it->first + "=" + it->second;
        envp[i] = strdup(entry.c_str());
    }
    envp[i] = NULL;
    return envp;
}

int CGIPost::executeScript(int fd) {

    int file_fd = open(this->file.c_str(), O_RDONLY);
    if (file_fd == -1) {
        throw std::runtime_error("Failed to open temp file: " + this->file);
    }
    std::cout << "passCGI  " << this->passCgi << std::endl;

    char* argv[3];
    argv[0] = (char*)this->passCgi.c_str();
    argv[1] = (char*)this->scriptPath.c_str();
    argv[2] = NULL;

    char **envp = buildEnvp(this->envVars);
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return 500;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 500;
    }

    if (pid == 0) {
        
        int inputFd = open(this->file.c_str(), O_RDONLY);
        if (inputFd < 0) {
            perror("Failed to open input file");
            exit(1);
        }
        
        dup2(inputFd, 0);
        close(inputFd);

        
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        close(pipefd[1]);

        execve(argv[0], argv, &envp[0]);

        
        perror("execve failed");
        exit(1);
    } else {
        close(pipefd[1]);

        char buffer[1024];
        ssize_t bytesRead;
        std::string output;

        // i need to remove this and fix it ??
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        
        // bytesRead = read(pipefd[0], buffer, sizeof(buffer));
        // if (bytesRead > 0) 
        //     output.append(buffer, bytesRead);
        

        close(pipefd[0]);
        wait(NULL);
        std::stringstream header;
        header  << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html\r\n"
            << "Connection: close\r\n"
            << "\r\n";
        send(fd, header.str().c_str(), header.str().length(), 0);


        std::string::size_type headerEnd = output.find("\r\n\r\n");
        std::string cgiHeaders = output.substr(0, headerEnd);
        std::string cgiBody = output.substr(headerEnd + 4);
        if (unlink(this->file.c_str()) == 0) {
            std::cout << "File deleted successfully.\n";
        } else {
            perror("Error deleting the file");
        }
 
        send(fd, cgiBody.c_str(), cgiBody.length(), 0);
        return 200;
    }
}