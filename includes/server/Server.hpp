#ifndef SERVER_HPP
#define SERVER_HPP

#include "../WebServ.hpp"
#include "ListeningSocket.hpp"
#include "../configFile/ServerBlock.hpp"

class Server {

  private:
    ServerBlock _serverBlock;
    std::vector<ListeningSocket> _listeningSocket;
    int _isDefaultServer;
    std::map<int, int> _serverStatusAccordingToPort;

  public:
    Server(const ServerBlock& serverBlock);
    ~Server();

    /* setters */

    void setDefaultServer(int status);

    /* getters */

    int getDefaultServer() const;
    std::vector<ListeningSocket>& getListeningSocket();
    ServerBlock getServerBlock() const;
    static std::vector<Server> getAllServers(const std::vector<ServerBlock>& serverBlocks);
    void addStatus(const std::map<int, int>& status);
    std::map<int, int> getServerStatusAccordingToPort() const;

    void printServerInfo() const;
};

#endif