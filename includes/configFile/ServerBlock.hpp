#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "../WebServ.hpp"
#include "Block.hpp"
#include "RecupBlockContent.hpp"
#include "LocationBlock.hpp"
#include "CgiHandler.hpp"
#include "HostHandler.hpp"

class HostHandler;

class ServerBlock {

    private:
       std::string _server;
       int _port;
       HostHandler _host;
       std::string _root;
       std::string _index;
       std::vector<LocationBlock> _location;
       std::vector<std::string> _allowMethods;
       std::vector<CgiHandler> _cgiExtension;
       std::string _client_max_body_size;

    public:
        ServerBlock();
        ~ServerBlock();

        /* setters */

        void setServer(std::string server);
        void setPort(int port);
        void setHost(const HostHandler& host);
        void setRoot(std::string root);
        void setIndex(std::string index);
        void setLocation(std::vector<LocationBlock> location);
        void setAllowMethods(std::vector<std::string> allowMethods);
        void setCgiExtension(std::vector<CgiHandler> cgiExtension);
        void setClientMaxBodySize(std::string client_max_body_size);
        
        /* getters */

        std::string getServer() const;
        int getPort() const;
        HostHandler getHost() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::vector<LocationBlock> getLocationBlock() const;
        std::vector<std::string> getAllowMethods() const;
        std::vector<CgiHandler> getCgiExtension() const;
        std::string getClientMaxBodySize() const;

        void addLocationBlock(const LocationBlock& location);
        void rootCheck();
        void indexCheck();

        void addHost(const HostHandler& host);

        /* ca va permettre d exploiter les blocks qui ont ete construit sous forme d arbre  */
        std::vector<ServerBlock> createAllServerBlocks(RecupBlockContent rawConfig);
        void caseWithNoLocationBlockEmbeded(ServerBlock& oneServerBlock, std::multimap<std::string, std::string> directive);
        void caseWithLocationBlockEmbeded(LocationBlock& locBlock, std::multimap<std::string, std::string> directive);
};

#endif