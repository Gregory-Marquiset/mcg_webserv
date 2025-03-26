#ifndef HOSTHANDLER_HPP
#define HOSTHANDLER_HPP

#include "../WebServ.hpp"

class HostHandler {

    private:
        std::string _hostName;
        int _hostFormat;

    public:
        HostHandler();
        ~HostHandler();

        void setHostName(std::string hostName);
        void setHostFormat(int _hostFormat);

        std::string getHostName() const;
        int getHostFormat() const;
        
        void filter(std::string hostLine);
        void parseIp(std::string hostLine);
};

#endif