#include "WebServer.hpp"

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
    event.events = EPOLLIN | EPOLLET;
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
    
    while (true) { 
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                break;
            }
        }
        
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
                  
        if (!setNonBlocking(client_fd)) {
            close(client_fd);
            continue;
        }
        
        if (!addToEpoll(client_fd)) {
            close(client_fd);
            continue;
        }
    }
}
// std::string& res = write_buffers[fd];

// ssize_t bytes_sent = send(fd, res.c_str(), res.length(), 0);

// if (bytes_sent == -1) {
//     if (errno == EAGAIN || errno == EWOULDBLOCK) {
//         // Socket buffer full, try again later
//         return;
//     }
//     std::cerr << "send failed: " << strerror(errno) << std::endl;
//     closeConnection(fd);
//     return;
// }

// // Success! Clear the buffer
// write_buffers.erase(fd);

// std::cout << "Response sent successfully to client: " << fd << std::endl;
// closeConnection(fd);
void WebServer::getResponse(int client_fd)
{
    const char* response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>Hello Page</title>\n"
    "    <style>\n"
    "        body {\n"
    "            display: flex;\n"
    "            justify-content: center;\n"
    "            align-items: center;\n"
    "            height: 100vh;\n"
    "            background-color: #f0f0f0;\n"
    "            font-family: Arial, sans-serif;\n"
    "        }\n"
    "        h1 {\n"
    "            color: #333;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>Hello</h1>\n"
    "</body>\n"
    "</html>";
    ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
    if (bytes_sent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Socket buffer full, try again later
            return;
        }
        std::cerr << "send failed: " << strerror(errno) << std::endl;
        close(client_fd);
        return;
    }
    std::cout << "Response sent successfully to client: " << client_fd << std::endl;
    close(client_fd);
}

void WebServer::handleClientData(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t count;
    
    while (true) {
        count = read(client_fd, buffer, sizeof(buffer) - 1);
        if (count == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } 
            else {
                std::cerr << "Read error: " << strerror(errno) << std::endl;
                close(client_fd);
                break;
            }
        } else if (count == 0) {
            std::cout << "Connection closed by client" << std::endl;
            close(client_fd);
            break;
        } else {
            buffer[count] = '\0';
            // const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 14\r\n\r\nHello, World1!";
            // write(client_fd, response, strlen(response));
            std::cout << "Received data: " << buffer << std::endl;
        }
    }
}

WebServer::WebServer() : epoll_fd(-1) {}
        
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
            std::cerr << "Failed to initialize server " << *it_names << std::endl;
        }
    }
    this->run();
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
        server_addr.sin_addr.s_addr = inet_addr(server->getHost().c_str()); 
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

void WebServer::run() {
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
                handleClientData(events[i].data.fd);
                getResponse(events[i].data.fd);
            }
            check = 0;
        }
    }
}