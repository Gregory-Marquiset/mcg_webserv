#ifndef HOSTHANDLER_HPP
#define HOSTHANDLER_HPP

#include "../WebServ.hpp"
#include "ServerBlock.hpp"

class ServerBlock;

class HostHandler {

    private:
        // std::vector<std::string> _hostName;
        std::string _hostName;
        int _hostFormat;

    public:
        HostHandler();
        ~HostHandler();

        // void setHostName(std::vector<std::string> hostName);
        void setHostName(std::string hostName);
        void setHostFormat(int _hostFormat);

        //std::vector<std::string> getHostName() const;
        std::string getHostName() const;
        int getHostFormat() const;
        
        void filter(std::string hostLine);
        void parseIp(std::string hostLine);
        void checkListenFormat(std::string listenLine, ServerBlock& server);
};

#endif