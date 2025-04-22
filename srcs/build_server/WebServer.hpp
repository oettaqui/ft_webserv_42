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

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024


class WebServer {
    private:
        int server_fd;            // Listening socket file descriptor
        int epoll_fd;             // epoll file descriptor
        struct epoll_event events[MAX_EVENTS];  // Events array for epoll_wait
        
        // Sets a socket to non-blocking mode
        bool setNonBlocking(int sockfd);
        
        // Adds a file descriptor to epoll
        bool addToEpoll(int sockfd);
        
        // Handles a new connection
        void handleNewConnection() ;
        
        // Handles data from a client
        void handleClientData(int client_fd);
    
    public:
        WebServer();
        
        ~WebServer() ;
        
        // Initialize the server
        bool initialize(int port) ;
        // Run the server
        void run();
    };