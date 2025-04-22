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

// Adds a file descriptor to epoll
bool WebServer::addToEpoll(int sockfd) {
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Set for read operation and edge triggered
    event.data.fd = sockfd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        std::cerr << "Failed to add fd to epoll: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

// Handles a new connection
void WebServer::handleNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (true) {  // Accept all pending connections
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more connections to accept
                break;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                break;
            }
        }
        
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
                  
        // Set the new socket to non-blocking
        if (!setNonBlocking(client_fd)) {
            close(client_fd);
            continue;
        }
        
        // Add to epoll
        if (!addToEpoll(client_fd)) {
            close(client_fd);
            continue;
        }
    }
}

// Handles data from a client
void WebServer::handleClientData(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t count;
    
    while (true) {  // Read all available data
        count = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if (count == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read
                break;
            } else {
                std::cerr << "Read error: " << strerror(errno) << std::endl;
                close(client_fd);
                break;
            }
        } else if (count == 0) {
            // Connection closed by client
            std::cout << "Connection closed by client" << std::endl;
            close(client_fd);
            break;
        } else {
            // Process the received data
            buffer[count] = '\0';  // Null-terminate the string
            std::cout << "Received data: " << buffer << std::endl;
            
            // Echo back for now (will be replaced with HTTP response later)
            const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
            write(client_fd, response, strlen(response));
        }
    }
}

WebServer::WebServer() : server_fd(-1), epoll_fd(-1) {}
        
WebServer::~WebServer() {
    if (server_fd != -1) close(server_fd);
    if (epoll_fd != -1) close(epoll_fd);
}

// Initialize the server
bool WebServer::initialize(int port) {
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set to non-blocking
    if (!setNonBlocking(server_fd)) {
        return false;
    }
    
    // Bind to port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Listen
    if (listen(server_fd, SOMAXCONN) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Create epoll instance
    epoll_fd = epoll_create(MAX_EVENTS);  // Using epoll_create as specified
    if (epoll_fd == -1) {
        std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Add server socket to epoll
    if (!addToEpoll(server_fd)) {
        return false;
    }
    
    std::cout << "Server initialized successfully on port " << port << std::endl;
    return true;
}

// Run the server
void WebServer::run() {
    std::cout << "Server running..." << std::endl;
    
    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        
        if (num_events == -1) {
            std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
            break;
        }
        
        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                // New connection
                handleNewConnection();
            } else {
                // Data from a client
                handleClientData(events[i].data.fd);
            }
        }
    }
}