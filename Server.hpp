
#ifndef SERVER_HPP
#define SERVER_HPP


#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "./ParsRequest/ParsRequest.hpp"


#define MAX_EVENTS 64
#define BUFFER_SIZE 1024
#define MAX_CONN 1000
#define PORT 8080


class Server{

    private:
        int listen_fd;
        int epoll_fd;
        struct epoll_event events[MAX_EVENTS];
        std::map<int, ParsRequest* > clients;
        std::map<int, std::string> write_buffers;
        
    
          

        
        void handleNewConnection();
        bool setNonBlocking(int sockfd);
        void handleRead(int fd);
        void handleWrite(int fd);
        void closeConnection(int fd);

    public:
        Server();
        ~Server();

        bool initialize();
        void run();

    
};



#endif