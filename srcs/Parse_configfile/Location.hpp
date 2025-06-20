#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
private:
    std::string path;               
    std::string root;  
    std::vector<std::string> methods;   
    std::vector<std::string> index;    
    bool autoindex;                  
    bool cgi;                     
    std::string upload_store;          
    size_t client_max_body_size;      
    std::map<std::string, std::string> cgi_pass; 
    std::map<int, std::string> redirection; 
    bool redirect;
public:
    Location();
    ~Location();
    void setPath(const std::string& p);
    void setRoot(const std::string& r);
    void addMethod(const std::string& method);
    void addIndex(const std::string& idx);
    void setAutoindex(bool value);
    void setCgi(bool value);
    void setUploadStore(const std::string& path);
    void setClientMaxBodySize(size_t size);
    void addCgiPass(const std::string& ext, const std::string& handler);
    const std::string& getPath() const;
    const std::string& getRoot() const;
    const std::vector<std::string>& getMethods() const;
    const std::vector<std::string>& getIndex() const;
    bool getAutoindex() const;
    bool getCgi() const;
    const std::string& getUploadStore() const;
    size_t getClientMaxBodySize() const;
    const std::map<std::string, std::string>& getCgiPass() const;
    bool isValid() const;
    void setRedirect();
    bool hasRedirect() const;
    void setRedirection(const int& status, const std::string& path);
    const std::map<int, std::string>&  getRedirection() const;
};

#endif