#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP

#include "../WebServ.hpp"
#include "../configFile/Block.hpp"
#include "../configFile/Parse.hpp"
#include "../configFile/utils.hpp"

class ConfigData {

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
        ConfigData();
        ~ConfigData();

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

        void setConfigData(Parse rawConfig);
      
};

#endif