#include "WebServer.hpp"
WebServer::WebServer() : epoll_fd(-1) {}
        
WebServer::~WebServer() {
    for (std::vector<int>::const_iterator it = server_fds.begin(); it != server_fds.end(); it++)
    {
        if (*it != -1) 
            close(*it);
    }
    if (epoll_fd != -1) close(epoll_fd);
}

bool WebServer::addToEpoll(int sockfd) {
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = sockfd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        std::cerr << "Failed to add fd to epoll: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}


bool WebServer::setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl F_GETFL failed" << std::endl;
        return false;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl F_SETFL failed" << std::endl;
        return false;
    }
    return true;
}


bool WebServer::initialize(std::vector<Server>::const_iterator &server) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        return false;
    }
    if (!setNonBlocking(server_fd)) {
        return false;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(strcmp("localhost",server->getHost().c_str()) == 0)
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    else if(strcmp("dump-ubuntu-benguerir",server->getHost().c_str()) == 0)
        server_addr.sin_addr.s_addr = inet_addr("127.0.1.1"); 
    else
    {
        std::cout << "*****> " << server->getHost().c_str() << "<*****" << std::endl;
        server_addr.sin_addr.s_addr = inet_addr(server->getHost().c_str()); 
    }
    server_addr.sin_port = htons(server->getPort());
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    }
    if (listen(server_fd, SOMAXCONN) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return false;
    }
    if(epoll_fd == -1)
    {
        epoll_fd = epoll_create(MAX_EVENTS);
        if (epoll_fd == -1) {
            std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
            return false;
        }
    }
    if (!addToEpoll(server_fd)) {
        return false;
    }
    server_fds.push_back(server_fd);
    std::vector<std::string>::const_iterator it_names = server->getServerNames().begin();
    std::cerr << "server " << *it_names << " is alive on port " << server->getPort() << " and host " << server->getHost() << std::endl;
    return true;
}

void WebServer::linking_servers(ConfigParser &parser)
{
    for (std::vector<Server>::const_iterator it = parser.getServers().begin(); it != parser.getServers().end(); ++it) {
        if (!this->initialize(it)) {
            std::vector<std::string>::const_iterator it_names = it->getServerNames().begin();
            if(it_names != it->getServerNames().end())
                std::cerr << "Failed to initialize server " << *it_names << std::endl;
            else
                std::cerr << "Failed to initialize server with host : " << it->getHost() << " and port : " << it->getPort() << std::endl;

        }
    }
    this->run(parser);
}

void WebServer::handleNewConnection(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // while (true) { 
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return ;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                return ;
            }
        }
        
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
                  
        if (!setNonBlocking(client_fd)) {
            close(client_fd);
        }
        
        if (!addToEpoll(client_fd)) {
            close(client_fd);
        }
        std::cout << "*1*\n";
        // clients[client_fd] = new ParsRequest();
    // }
}

void WebServer::handleClientData(int fd, ConfigParser &parser) {

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read;
    
    while (true) {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            if (errno == EAGAIN  || errno != EWOULDBLOCK) {
                std::cerr << "Failed read operation " << std::endl;
                break;
            }
            else
            {
                write_buffers[fd] = HTML_BADREQUEST;
                // closeConnection(fd);
                std::cout << "Client disconnected: hhhh" << fd << std::endl;
                break;

            }
        }
        else {
            std::string req;
            req.append(buffer, bytes_read);
            ParsRequest* p = clients[fd];
            p->parse(req,fd, parser);
            if(!p->isValid())
            {
                if(p->getMethod() == "GET")
                    write_buffers[fd] = p->getResponses().find(fd)->second;
                else if(p->getMethod() == "DELETE")
                    write_buffers[fd] = p->getResponses().find(fd)->second;
                else
                    write_buffers[fd] = HTML_BADREQUEST;
                return;
            }
            
            if (p->isComplet()) {
                std::cout << "Complete request received, preparing response" << std::endl;
                std::cout << "Method : |" << p->getMethod() << "|" << std::endl;
                if(p->getMethod() == "GET")
                    write_buffers[fd] = p->getResponses().find(fd)->second;
                else if(p->getMethod() == "DELETE")
                    write_buffers[fd] = p->getResponses().find(fd)->second;
                else
                    write_buffers[fd] = HTML_RESPONSE;
                break;
            }
        }
    }
}

void WebServer::run(ConfigParser &parser) {
    (void)parser;
    std::cout << "Server(s) running..." << std::endl;
    int check = 0;
    while (true) 
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
            break;
        }
        for (int i = 0; i < num_events; i++) {
            for (std::vector<int>::const_iterator it = server_fds.begin(); it != server_fds.end(); ++it)
            {
                if (events[i].data.fd == *it) {
                    handleNewConnection(*it);
                    check = 1;
                }
            }
            if(check == 0) {
                std::cout << "lahbaaaaal 👍👓\n";
                // handleClientData(events[i].data.fd, parser);
                // getResponse(events[i].data.fd);
            }
            check = 0;
        }
    }
}