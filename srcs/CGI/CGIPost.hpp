#ifndef CGIPOST_HPP
#define CGIPOST_HPP


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

class CGIPost{

private:


    std::map<std::string, std::string> envVars;
    std::string toString(size_t n);
    std::string scriptPath;
    std::string file;
    std::string passCgi;
    std::string autoIndex;

public:
    CGIPost();
    ~CGIPost();

    void setVarsEnv(dataCGI& data);
    char** buildEnvp(std::map<std::string, std::string>& env);
    int executeScript(int fd);

};


#endif


