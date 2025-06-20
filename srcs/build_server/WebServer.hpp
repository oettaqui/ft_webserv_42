#include <iostream>
#include <string>
#include <csignal>
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
#include "../pars_request/ParsRequest.hpp"

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

#define REQUEST_TIMEOUT 30000
#define CLIENT_TIMEOUT_MS 60000

class WebServer {
    private:

        std::vector<int>  server_fds;
        int epoll_fd;
        struct epoll_event events[MAX_EVENTS];
        std::map<int, std::string > req_clients;
        std::map<int, ParsRequest* > clients;
        std::map<int, std::string> write_buffers;
        
        std::map<int, time_t> client_request_start;
        std::map<int, time_t> client_last_activity;
        std::map<int,bool> clients_mode;
        
        bool setNonBlocking(int sockfd);
        bool addToEpoll(int sockfd,int flag);
        void handleNewConnection(int server_fd);
        void handleClientData(int fd, ConfigParser &parser);
        void getResponse(int fd,ConfigParser &parser);
        void closeConnection(int fd);
        long getCurrentTimeMs();
    public:
        static WebServer* instance;
        bool running;
        WebServer();
        ~WebServer() ;
        bool initialize(std::vector<Server>::const_iterator &server) ;
        void linking_servers(ConfigParser &parser) ;
        void run(ConfigParser &parser);
        void checkInactiveClients();
        bool isSocketAlive(int sockfd);
        void sig();
        static void signalHandler(int signum);
        void cleanup();
    };