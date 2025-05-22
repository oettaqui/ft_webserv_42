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


    std::string leftoverData;
    std::string extension;
    std::string headers;
    std::string content;
    int check;

    std::string boundarySep;
    std::string sep;
    std::string terminator;

    // CGI
    bool isCGI;
    std::string cType;
    std::string scriptPath;
    std::map<std::string, std::string> cgiPassMap;

    
    
public:
    PostHandler();
    ~PostHandler();
    

    void initialize(ParsRequest &data_req, ConfigParser &parser);
    
    void processData(const std::string& data);

    void processChunkedData(const std::string& data);

    void initBoundary(const std::string& initBody, ParsRequest &data_req, ConfigParser &parser);

    void processBoundaryData(std::string& initBody, ParsRequest &data_req, std::string& location_path);


    bool isRequestComplete() const;

    const std::string& getBody() const;
    const std::string& getFilename() const;
    size_t getCurrentLength() const;

    bool directoryExists(const std::string& path);
    int getStatus() const;
    void setExpextedLength(size_t len);

    std::string extractContentType(const std::string& headers);

    void setSepa(std::string sep);
    void setTer(std::string ter);


    bool getCGIState() const;
    std::string getContentType() const;
    std::string getScriptPath() const;
    std::map<std::string, std::string> getCgiPass() const;
};

#endif