#ifndef POST_HANDLER_HPP
#define POST_HANDLER_HPP

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "../Parse_configfile/ConfigParser.hpp"
#include "../Parse_configfile/Server.hpp"
#include "../pars_request/ParsRequest.hpp"

class ParsRequest;
class PostHandler {
private:
    std::string body;
    std::ofstream file;
    std::string filename;
    size_t bodyLength;
    size_t expectedLength;
    bool isComplete;
    std::map<std::string, std::string> contentTypes;

    std::string createUniqueFile(const std::string& extension);
    void storeContentTypes();



    bool isChunked;
    int currentChunkSize;
    int currentChunkBytesRead; 
    std::string chunkSizeBuffer;
    enum ChunkState {
        READING_SIZE,
        READING_DATA,
        READING_TRAILING_CRLF,
        END_OF_CHUNKS
    };
    ChunkState chunkState;

    // Server server;
    
public:
    PostHandler();
    ~PostHandler();
    

    void initialize(ParsRequest &data_req, ConfigParser &parser);
    
    void initBoundary(const std::string& initBody,  const std::string &boundaryValue);

    void processData(const std::string& data);

    void processChunkedData(const std::string& data);

    void processBoundaryData(const std::string& data, const std::string &boundaryValue);
    
    std::string extractFormFieldValue(const std::string& body, const std::string& boundary);

    bool isRequestComplete() const;

    const std::string& getBody() const;
    const std::string& getFilename() const;
    size_t getCurrentLength() const;
};


#endif