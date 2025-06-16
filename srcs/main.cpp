#include <iostream>
#include "Parse_configfile/ConfigParser.hpp"
#include "build_server/WebServer.hpp"

int main(int ac,char **av) {
    if (ac != 2 && ac != 1) {
        std::cerr << "Usage (1): ./Webserv \"configuration_file\" 🐸" << std::endl;
        std::cerr << "Usage (2): ./Webserv 🐸" << std::endl;
        return 1;
    }
    else
    {
        std::string path_conf;
        if(ac == 1)
            path_conf = "./config/webserv.conf";
        else
            path_conf = av[1];
        try
        {
            ConfigParser parser;
            if (!parser.parse(path_conf)) {
                std::cerr << "⛔Failed to parse configuration⛔" << std::endl;
                return 1;
            }
            else
            {
                parser.prapare_servers();
                WebServer server;
                server.linking_servers(parser);
            }
        }
        catch(const char *str)
        {
            std::cerr << str << std::endl;
            return 1;
        }   
    }
    
    return 1;
}
