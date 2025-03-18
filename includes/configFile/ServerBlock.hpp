#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "../WebServ.hpp"
#include "Block.hpp"
#include "RecupBlockContent.hpp"
#include "LocationBlock.hpp"

class ServerBlock {

    private:
       std::string _server;
       int _listen;
       std::string _serverName;
       std::vector<LocationBlock> _location;
       std::string _root;
       std::string _index;
       std::string _allowMethods;
       std::string _cgiExtension;
       std::string _client_max_body_size;

    public:
        ServerBlock();
        ~ServerBlock();

        /* setters */

        void setServer(std::string server);
        void setListen(int listen);
        void setServerName(std::string serverName);
        void setLocation(std::vector<LocationBlock> location);
        void setRoot(std::string root);
        void setIndex(std::string index);
        void setAllowMethods(std::string allowMethods);
        void setCgiExtension(std::string cgiExtension);
        void setClientMaxBodySize(std::string client_max_body_size);
        
        /* getters */

        std::string getServer() const;
        int getListen() const;
        std::string getServerName() const;
        std::vector<LocationBlock> getLocation() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::string getAllowMethods() const;
        std::string getCgiExtension() const;
        std::string getClientMaxBodySize() const;

        void addLocationBlock(const LocationBlock& location);

        /* ca va permettre d exploiter les blocks qui ont ete construit sous forme d arbre  */
        std::vector<ServerBlock> getAllServerBlocks(RecupBlockContent rawConfig);
};

#endif