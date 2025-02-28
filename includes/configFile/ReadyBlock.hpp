#ifndef READYBLOCK_HPP
#define READYBLOCK_HPP

#include "../WebServ.hpp"
#include "Block.hpp"
#include "RecupBlockContent.hpp"
// #include "../configFile/utils.hpp"

class ReadyBlock {

    private:
       std::string _server;
       int _listen;
       std::string _serverName;
       std::string _location;
       std::string _root;
       std::string _index;
       std::string _allowMethods;
       std::string _cgiExtension;

    public:
        ReadyBlock();
        ~ReadyBlock();

        /* setters */

        void setServer(std::string server);
        void setListen(int listen);
        void setServerName(std::string serverName);
        void setLocation(std::string location);
        void setRoot(std::string root);
        void setIndex(std::string index);
        void setAllowMethods(std::string allowMethods);
        void setCgiExtension(std::string cgiExtension);
        
        /* getters */

        std::string getServer() const;
        int getListen() const;
        std::string getServerName() const;
        std::string getLocation() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::string getAllowMethods() const;
        std::string getCgiExtension() const;

        /* set the full object config data */

        void setReadyBlock(RecupBlockContent rawConfig);
      
};

#endif