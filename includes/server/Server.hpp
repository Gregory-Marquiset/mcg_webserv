#ifndef SERVER_HPP
#define SERVER_HPP

#include "../WebServ.hpp"
#include "ListeningSocket.hpp"
#include "../configFile/ServerBlock.hpp"

class Server {

  private:
    ServerBlock _serverBlock;
    ListeningSocket _listeningSocket;
    int _isDefaultServer;

  public:
    Server(const ServerBlock& serverBlock);
    ~Server();

    /* setters */

    void setDefaultServer(int status);

    /* getters */

    int getDefaultServer() const;
    ListeningSocket getListeningSocket() const;
    ServerBlock getServerBlock() const;
    static std::vector<Server> getAllServers(const std::vector<ServerBlock>& serverBlocks);

    void printServerInfo() const;

    // /* path handler */
    // void pathHandler()
};

#endif