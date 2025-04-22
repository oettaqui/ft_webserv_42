// #include <iostream>
// #include "Parse_configfile/ConfigParser.hpp"

// int main(int ac,char **av) {
//     if (ac != 2) {
//         std::cerr << "Usage: ./Webserv \"configuration_file\" 🐸" << std::endl;
//         return 1;
//     }
//     else
//     {
//         try
//         {
//             ConfigParser parser;
//             if (!parser.parse(av[1])) {
//                 std::cerr << "⛔Failed to parse configuration⛔" << std::endl;
//                 return 1;
//             }
//             else
//             {
//                 parser.all_server_data();
//                 std::cout << "succeed to parse configuration" << std::endl;
//                 std::cout << "servers number : " << parser.getServers().size() << std::endl;
//             }
//         }
//         catch(const char *str)
//         {
//             std::cerr << str << std::endl;
//             return 1;
//         }   
//     }
    
//     return 0;
// }
#include "build_server/WebServer.hpp"

int main(int argc, char* argv[]) {
    int port = 8080;  // Default port
    
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    WebServer server;
    if (!server.initialize(port)) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
    
    server.run();
    
    return 0;
}