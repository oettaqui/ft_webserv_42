#include "WebServer.hpp"

const std::string HTML_RESPONSE = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n\r\n"
"<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "    <title>Hello World</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "    <h1>Hello World</h1>\r\n"
    "</body>\r\n"
    "</html>\r\n";


const std::string HTML_BADREQUEST = 
"HTTP/1.1 400 Bad Request\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n\r\n"
"<!DOCTYPE html>\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "    <title>Bad Request</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "    <h1>Bad Request11</h1>\r\n"
    "</body>\r\n"
    "</html>\r\n";

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


bool WebServer::addToEpoll(int sockfd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        std::cerr << "Failed to add fd to epoll: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void WebServer::handleNewConnection(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                return;
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
        clients[client_fd] = new ParsRequest();
}

void WebServer::getResponse(int fd, ConfigParser &parser)
{
    std::cout << "getResponse\n";
    ParsRequest* p = clients[fd];
    if(p)
    {
        if(p->getMethod() == "GET" && p->isComplet() == false)
            p->parse("",fd, parser);
        else
        {
            if (!(write_buffers.find(fd) == write_buffers.end())) {
                std::string& res = write_buffers[fd];
                ssize_t bytes_sent = send(fd, res.c_str(), res.length(), 0);
                if (bytes_sent == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        return;
                    }
                    std::cerr << "send failed: " << strerror(errno) << std::endl;
                    closeConnection(fd);
                    return;
                }
                write_buffers.erase(fd);
            }
            
            std::cout << "Response sent successfully to client: " << fd << std::endl;
            struct epoll_event event;
            event.events = EPOLLIN;
            event.data.fd = fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
            closeConnection(fd);
        }
    }
    else
    {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
        closeConnection(fd);
    }
}
void WebServer::handleClientData(int fd, ConfigParser &parser) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);

    if (bytes_read <= 0) {
        if (bytes_read < 0 && errno != EAGAIN) {
            std::cerr << "Failed read operation " << std::endl;
        }
        closeConnection(fd);
        std::cout << "Client disconnected: " << fd << std::endl;
        return;
    }
    else
    {
        std::string req;
        req.append(buffer, bytes_read);
        ParsRequest* p = clients[fd];
        p->parse(req,fd, parser);
        if(!p->isValid())
        {
            std::cout << !p->isValid() << std::endl;
            if(p->getMethod() == "GET")
            {
                if (!p->getResponses().find(fd)->second.empty())
                    write_buffers[fd] = p->getResponses().find(fd)->second;
            }
            else if(p->getMethod() == "DELETE")
                write_buffers[fd] = p->getResponses().find(fd)->second;
            else
                write_buffers[fd] = HTML_BADREQUEST;
            struct epoll_event event;
            event.events = EPOLLOUT;
            event.data.fd = fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
            return;
        }
        if (p->isComplet()) 
        {
            if(p->getMethod() == "GET")
            {
                std::cout << "**********<<\n";
                write_buffers[fd] = p->getResponses().find(fd)->second;
            }
            else if(p->getMethod() == "DELETE")
                write_buffers[fd] = p->getResponses().find(fd)->second;
            else if(!p->getCGIState())
                write_buffers[fd] = HTML_RESPONSE;
            struct epoll_event event;
            event.events = EPOLLOUT;
            event.data.fd = fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
            return ;
        }
        else if(p->getMethod() == "GET")
        {
            struct epoll_event event;
            event.events = EPOLLOUT;
            event.data.fd = fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
        }
        
    }
}

WebServer::WebServer() : epoll_fd(-1)  {}
        
WebServer::~WebServer() {
    for (std::vector<int>::const_iterator it = server_fds.begin(); it != server_fds.end(); it++)
    {
        if (*it != -1) 
            close(*it);
    }
    if (epoll_fd != -1) close(epoll_fd);
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
    if(it_names != server->getServerNames().end())
        std::cerr << "server " << *it_names << " is alive on port " << server->getPort() << " and host " << server->getHost() << std::endl;
    else
        std::cerr << "this server is alive on port " << server->getPort() << " and host " << server->getHost() << std::endl;
    return true;
}

void WebServer::closeConnection(int fd) {
    std::cout << "Closing connection fd: " << fd << std::endl;
    

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);

    shutdown(fd,SHUT_RDWR);
    

    close(fd);
    

    if (clients.find(fd) != clients.end()) {
        delete clients[fd];
        clients.erase(fd);
    }
    write_buffers.erase(fd);
}

void WebServer::run(ConfigParser &parser) {
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
                if (events[i].events & EPOLLIN) {
                    handleClientData(events[i].data.fd, parser);
                }
                if(events[i].events & EPOLLOUT)
                {
                    getResponse(events[i].data.fd,parser);;
                }
                if ((events[i].events & EPOLLERR) || 
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN) && !(events[i].events & EPOLLOUT))) {
                    closeConnection(events[i].data.fd);
                }
            }
            check = 0;
        }
    }
}