#ifndef POST_HANDLER_HPP
#define POST_HANDLER_HPP

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include <sys/stat.h>
#include "../Parse_configfile/ConfigParser.hpp"
#include "../Parse_configfile/Server.hpp"
#include "../pars_request/ParsRequest.hpp"
#include "../Parse_configfile/Location.hpp"

class ParsRequest;
class PostHandler {
private:
    std::string body;
    std::ofstream file;
    std::string filename;
    size_t bodyLength;
    size_t expectedLength;
    bool isComplete;
    size_t maxBodySize;
    std::map<std::string, std::string> contentTypes;

    std::string createUniqueFile(const std::string& extension, std::string& location_path);
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
    int status;
    std::map<std::string, Location> locations;


     enum BoundaryState {
        START_SEPARATOR,    // Finding initial boundary
        READING_HEADERS,    // Reading headers after boundary
        READING_CONTENT,    // Reading content
        NEXT_SEPARATOR,     // Found boundary, deciding if it's a new part or terminator
        END_SEPARATOR       // Found terminator
    };
    BoundaryState boundaryState;

    std::string leftoverData;
    std::string extension;
    std::string header;
    std::string content;
    
public:
    PostHandler();
    ~PostHandler();
    

    void initialize(ParsRequest &data_req, ConfigParser &parser);
    
    void processData(const std::string& data);

    void processChunkedData(const std::string& data);

    void initBoundary(const std::string& initBody, const std::string &boundaryValue, ParsRequest &data_req, ConfigParser &parser);

    void processBoundaryData(const std::string& initBody, const std::string &boundarySep, ParsRequest &data_req, std::string& location_path);


    bool isRequestComplete() const;

    const std::string& getBody() const;
    const std::string& getFilename() const;
    size_t getCurrentLength() const;

    bool directoryExists(const std::string& path);
    int getStatus() const;
    void setExpextedLength(size_t len);

    std::string extractContentType(const std::string& headers);
};

#endif