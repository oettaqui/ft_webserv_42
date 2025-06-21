#include "./CGI.hpp"

CGI::CGI(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    this->flag = 0;
    this->status = 200;
    this->timeoutSeconds = 10;
    this->startTime = getCurrentTimeMs();
    size = 0;
    totat_bytes_read = 0;
    this->envp = NULL;

}

CGI::~CGI(){
    if (flag >= 1 && pipeFd[0] >= 0) {
        close(pipeFd[0]);
        close(pipeFd[1]);
    }
    if (this->envp)
    {
        for (size_t i = 0; this->envp[i]; ++i)
            free(this->envp[i]);
        delete[] this->envp;
    }
   
}

ssize_t CGI::getFileSize(int fd) {
    int bytes_available;
    if (ioctl(fd, FIONREAD, &bytes_available) == 0) {
        return static_cast<ssize_t>(bytes_available);
    }
    return -1;
}

void CGI::setVarsEnv(dataCGI& data) {

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
    if (data.headers.find("Cookie") != data.headers.end()) {
        envVars["HTTP_COOKIE"] = data.headers["Cookie"];
    }
    this->file = data.file;
    this->scriptPath = data.scriptPath;
    this->passCgi = data.CorrectPassCGI;
    this->autoIndex = data.autoIndex;


}


std::string CGI::toString(size_t n) {
        std::ostringstream oss;
        oss << n;
        return oss.str();
}

char** CGI::buildEnvp(std::map<std::string, std::string>& env) {
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



std::string CGI::executeScript(){

    if (flag >= 0 && flag <= 4) {
        if (isTimeout()) {
            if (pid > 0) {
                kill(pid, SIGTERM);
                kill(pid, SIGKILL);
                waitpid(pid, NULL, 0);
            }
            close(pipeFd[0]);
            close(pipeFd[1]);
            
            
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                    std::cerr << "Error deleting the file" << std::endl;
                }
            }
            
            flag = 5;
            this->status = 504;
            return "HTTP/1.1 504 Gateway Timeout\r\n\r\nCGI script execution timeout";
        }
    }

    if (flag == 0){
        if (envVars["REQUEST_METHOD"] == "POST") {
            if (access(this->file.c_str(), R_OK) == -1) {
            if (envVars["REQUEST_METHOD"] == "POST") {
               if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                }
            }
                this->status = 500;
                flag = 5;
                return "";
            }
        }
        if (access(this->passCgi.c_str(), X_OK) == -1) {
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                }
            }
            this->status = 500;
            flag = 5;
            return "";
        }
        if (pipe(pipeFd) == -1){
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                }
            }
            flag = 5;
            this->status = 500;
            return "";
        }
        flag = 1;
    }else if (flag == 1)
    {
        this->envp = buildEnvp(this->envVars);
        char* argv[3];
        argv[0] = (char*)this->passCgi.c_str();
        argv[1] = (char*)this->scriptPath.c_str();
        argv[2] = NULL;
        this->pid = fork();
        if (this->pid == -1) {
            std::cerr << "Error fork failed" << std::endl;
            this->status = 500;
            for (size_t i = 0; envp[i]; ++i)
                free(envp[i]);
            delete[] envp;
            flag = 5;
            return "";
        }
        
        if (pid == 0) {
            
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (freopen(this->file.c_str(), "r", stdin) == NULL) {
                    std::cerr << "Failed to redirect stdin with freopen" << std::endl;
                    exit(1);
                }
            }
            close(pipeFd[0]);
            dup2(pipeFd[1], 1);
            close(pipeFd[1]);
            execve(argv[0], argv, &envp[0]);
            perror("execve failed");
            exit(1);
            
        }
        flag = 2;
    }else if (flag == 2){
        
        int status;
        pid_t result = waitpid(this->pid, &status, WNOHANG);
        
        if (result > 0) {
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                this->status = 500;
                if (envVars["REQUEST_METHOD"] == "POST") {
                   if (unlink(this->file.c_str()) != 0) {
                        perror("Error deleting the file");
                    }
                }
                flag = 5;
                return "";
            }
            flag = 3; 
        } else if (result == -1) {
            std::cerr << "Error waitpid failed" << std::endl;
            this->status = 500;
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                }
            }
            flag = 5;
            return "";
        }
        
        if (flag == 3)
        {
            if (envVars["REQUEST_METHOD"] == "POST") {
                if (unlink(this->file.c_str()) != 0) {
                    perror("Error deleting the file");
                }
            }
        }
    }
    else if (flag == 3){
        size = getFileSize(pipeFd[0]);
        char buffer[1024];
        ssize_t bytesRead;
        std::string output = "";

        
        bytesRead = read(pipeFd[0], buffer, sizeof(buffer));
        if (bytesRead > 0) 
        output.append(buffer, bytesRead);
        if (bytesRead <= 0 || output.empty())
        {
            flag = 5;
            return "";
        }
        std::string response;
        size_t headerEnd = output.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            headerEnd = output.find("\n\n");
            if (headerEnd != std::string::npos) {
                std::string cgiHeaders = output.substr(0, headerEnd);
                std::string cgiBody = output.substr(headerEnd + 2);
                
                response = "HTTP/1.1 200 OK\r\n" + cgiHeaders + "\r\n\r\n" + cgiBody;
            }
        } else {
            std::string cgiHeaders = output.substr(0, headerEnd);
            std::string cgiBody = output.substr(headerEnd + 4);
            
            response = "HTTP/1.1 200 OK\r\n" + cgiHeaders + "\r\n\r\n" + cgiBody;
        }
        
        totat_bytes_read = totat_bytes_read + bytesRead;
        flag = 4;
        if (bytesRead < 1024 || totat_bytes_read >= size)
            flag = 5;
        return response; 
            
    }else if (flag == 4){
        char buffer[1024];
        ssize_t bytesRead;
        std::string output = "";
        bytesRead = read(pipeFd[0], buffer, sizeof(buffer));
        if (bytesRead > 0) 
            output.append(buffer, bytesRead);
        if (bytesRead <= 0 || output.empty())
        {
            flag = 5;
            return "";
        }
        totat_bytes_read = totat_bytes_read + bytesRead;
        if (bytesRead < 1024 || totat_bytes_read >= size)
            flag = 5;
        return output;
    }
    return "";  
}


int CGI::getCGIFlag() const{
    return flag;
}
int CGI::getStatusCGI() const{
    return status;
}


long CGI::getCurrentTimeMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

bool CGI::isTimeout() {
    long currentTime = getCurrentTimeMs();
    return (currentTime - startTime) > (timeoutSeconds * 1000);
}

void CGI::setTimeout(int seconds) {
    this->timeoutSeconds = seconds;
}