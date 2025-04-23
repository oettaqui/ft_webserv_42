#include "./Server.hpp"

// HTML response to send to clients
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
    
    Server::Server() : listen_fd(-1), epoll_fd(-1) {}
    
    
    Server::~Server() {
        if (listen_fd != -1) close(listen_fd);
        if (epoll_fd != -1) close(epoll_fd);
        
        // Clean up any remaining clients
        std::map<int, ParsRequest*>::iterator it;
        for (it = clients.begin(); it != clients.end(); ++it) {
            delete it->second;  // Free the ParsRequest memory
            close(it->first);   // Close the socket
        }
    }

    bool Server::setNonBlocking(int sockfd) {
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags == -1) {
            perror("fcntl F_GETFL");
            return false;
        }
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
            perror("fcntl F_SETFL O_NONBLOCK");
            return false;
        }
        return true;
    }

    void Server::handleNewConnection() {

        struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_fd == -1) {
                std::cerr << "accept failed " << strerror(errno) << std::endl;
                return;
            }
            
        
            setNonBlocking(client_fd);
            
            struct epoll_event event;
            event.events = EPOLLIN;
            event.data.fd = client_fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                std::cerr << "epoll_ctl client_fd: " << strerror(errno) << std::endl;
                close(client_fd);
                return;
            }
            clients[client_fd] = new ParsRequest();
            
            std::cout << "New client connected: " << client_fd << std::endl;
    }
    
    
    void Server::handleRead(int fd) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    
        if (bytes_read <= 0) {
            if (bytes_read < 0 && errno != EAGAIN) {
                std::cerr << "Failed read operation " << std::endl;
            }
            closeConnection(fd);
            std::cout << "Client disconnected: " << fd << std::endl;
            return;
        }
        else {
            std::string req;
            req.append(buffer, bytes_read);
            ParsRequest* p = clients[fd];
            p->parse(req);
            
            if (p->isComplet()) {
                std::cout << "Complete request received, preparing response" << std::endl;
                write_buffers[fd] = HTML_RESPONSE;
                
                // Register for write event
                struct epoll_event event;
                event.events = EPOLLOUT;
                event.data.fd = fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
            }
        }
    }
    
    void Server::handleWrite(int fd) {
        if (write_buffers.find(fd) == write_buffers.end()) {
            return;
        }
        
        std::string& res = write_buffers[fd];
        
        ssize_t bytes_sent = send(fd, res.c_str(), res.length(), 0);
        
        if (bytes_sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Socket buffer full, try again later
                return;
            }
            std::cerr << "send failed: " << strerror(errno) << std::endl;
            closeConnection(fd);
            return;
        }
        
        // Success! Clear the buffer
        write_buffers.erase(fd);
        
        std::cout << "Response sent successfully to client: " << fd << std::endl;
        closeConnection(fd);
        
        // Switch back to read mode
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    }
    
   
    
   


    void Server::closeConnection(int fd) {
        std::cout << "Closing connection fd: " << fd << std::endl;
        
        // Remove from epoll
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        
        // Close socket
        close(fd);
        
        // Clean up resources
        if (clients.find(fd) != clients.end()) {
            delete clients[fd];  // Free the ParsRequest memory
            clients.erase(fd);
        }
        write_buffers.erase(fd);
    }




    bool Server::initialize() {
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0) {
            perror("socket");
            return false;
        }

        int opt = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("setsockopt");
            close(listen_fd);
            return false;
        }

        if (!setNonBlocking(listen_fd)) {
            close(listen_fd);
            return false;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(PORT);

        if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("bind");
            close(listen_fd);
            return false;
        }

        if (listen(listen_fd, SOMAXCONN) < 0) {
            perror("listen");
            close(listen_fd);
            return false;
        }

        epoll_fd = epoll_create(MAX_CONN);
        if (epoll_fd < 0) {
            perror("epoll_create");
            close(listen_fd);
            return false;
        }

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = listen_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
            perror("epoll_ctl: listen_fd");
            close(listen_fd);
            close(epoll_fd);
            return false;
        }

        std::cout << "Server initialized on port " << PORT << std::endl;
        return true;
    }

    void Server::run() {
        while (true) {
            int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // 1 second timeout
            if (nfds < 0) {
                perror("epoll_wait");
                break;
            }

            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd == listen_fd) {
                    handleNewConnection();
                    // std::cout << "inside the main loop" << std::endl;
                    continue;
                }
                
                if (events[i].events & EPOLLIN) {
                    handleRead(events[i].data.fd);
                }
                
                if (events[i].events & EPOLLOUT) {
                    handleWrite(events[i].data.fd);
                }
                
                // Handle errors or connection closed
                if ((events[i].events & EPOLLERR) || 
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN) && !(events[i].events & EPOLLOUT))) {
                    closeConnection(events[i].data.fd);
                }
            }
        }
    }




