
#include "./src/build_server/Server.hpp"
#define MAX_CLIENTS 30


int main() {
    Server server;
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
    
    server.run();
    return 0;
}