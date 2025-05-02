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

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024


class WebServer {
    private:
        std::vector<int>  server_fds;
        int epoll_fd;
        struct epoll_event events[MAX_EVENTS];
        bool setNonBlocking(int sockfd);
        bool addToEpoll(int sockfd);
        void handleNewConnection(int server_fd);
        void handleClientData(int client_fd);
        void getResponse(int client_fd);
    public:
        WebServer();
        ~WebServer() ;
        bool initialize(std::vector<Server>::const_iterator &server) ;
        void linking_servers(ConfigParser &parser) ;
        void run();
    };