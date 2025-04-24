#ifndef POST_HANDLER_HPP
#define POST_HANDLER_HPP

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <sstream>

class PostHandler {
private:
    std::string body;
    std::ofstream file;
    std::string filename;
    size_t bodyLength;
    size_t expectedLength;
    bool isComplete;
    std::map<std::string, std::string> contentTypes;

    // Helper methods
    std::string createUniqueFile(const std::string& extension);
    void storeContentTypes();
    
public:
    PostHandler();
    ~PostHandler();
    
    // Initialize with expected content length
    void initialize(const std::string& contentType, size_t expectedLength, const std::string& initialBody = "");
    
    // Process more data
    void processData(const std::string& data);
    
    // Status checking
    bool isRequestComplete() const;
    
    // Getters
    const std::string& getBody() const;
    const std::string& getFilename() const;
    size_t getCurrentLength() const;
};

#endif // POST_HANDLER_HPP