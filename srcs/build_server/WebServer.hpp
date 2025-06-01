#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <cstdlib>
#include "../Parse_configfile/ConfigParser.hpp"
// #include "../pars_request/ParsRequest.hpp"

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024


class WebServer {
    private:
        std::vector<int>  server_fds;
        int epoll_fd;
        struct epoll_event events[MAX_EVENTS];
        // std::map<int, ParsRequest* > clients;
        std::map<int, std::string> write_buffers;
        bool setNonBlocking(int sockfd);
        bool addToEpoll(int sockfd);
        void handleNewConnection(int server_fd);
        void handleClientData(int fd, ConfigParser &parser);
        void getResponse(int fd);
        void closeConnection(int fd);
    public:
        WebServer();
        ~WebServer() ;
        bool initialize(std::vector<Server>::const_iterator &server) ;
        void linking_servers(ConfigParser &parser) ;
        void run(ConfigParser &parser);
    };