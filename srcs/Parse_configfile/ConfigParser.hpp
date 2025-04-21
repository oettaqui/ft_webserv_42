#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <string>
#include "Server.hpp"
#include <cstdlib>

class ConfigParser {
    private:
        std::vector<Server> servers;
        void trim(std::string& str);
        bool parseServer(std::ifstream& file, Server& server);
        bool parseLocation(std::ifstream& file, const std::string& line, Server& server);
        bool parseLine(const std::string& line, Server& server);
        bool validateConfig() const;

    public:
        ConfigParser();
        ~ConfigParser();
        bool parse(const std::string& filename);
        const std::vector<Server>& getServers() const;
        void all_server_data() const;
};

#endif