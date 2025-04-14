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
    std::vector<int> _clientsFd;

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

    void printServerInfo() const;
    void addStatus(const std::map<int, int>& status);
    std::map<int, int> getServerStatusAccordingToPort() const;
    void addClientToServerList(int newClientFd);
    // std::vector<int> getClientsFd() const;
};

#endif