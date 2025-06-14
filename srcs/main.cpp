#include <iostream>
#include "Parse_configfile/ConfigParser.hpp"
#include "build_server/WebServer.hpp"

int main(int ac,char **av) {
    if (ac != 2) {
        std::cerr << "Usage: ./Webserv \"configuration_file\" 🐸" << std::endl;
        return 1;
    }
    else
    {
        try
        {
            ConfigParser parser;
            if (!parser.parse(av[1])) {
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
