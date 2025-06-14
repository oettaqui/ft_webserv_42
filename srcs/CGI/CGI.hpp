#ifndef CGI_HPP
#define CGI_HPP


#include <map>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <errno.h>
#include "../pars_request/ParsRequest.hpp"
#include "./DataCGI.hpp"

class CGI{

private:


    std::map<std::string, std::string> envVars;
    std::string toString(size_t n);
    std::string scriptPath;
    std::string file;
    std::string passCgi;
    std::string autoIndex;
    int status;
    int flag;
    pid_t pid;
    int pipeFd[2];

    long startTime;
    int timeoutSeconds;


public:
    CGI();
    ~CGI();

    void setVarsEnv(dataCGI& data);
    char** buildEnvp(std::map<std::string, std::string>& env);
    std::string executeScript();
    int getStatusCGI() const;
    int getCGIFlag() const;

    long getCurrentTimeMs();
    bool isTimeout();
    void setTimeout(int seconds);

};


#endif


