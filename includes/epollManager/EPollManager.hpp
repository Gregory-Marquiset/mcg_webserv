#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../WebServ.hpp"
#include "../server/Server.hpp"

class EPollManager {
  private:
    std::vector<Server>& _servers;
    int _epollFd;
    std::vector<struct epoll_event> _events;
    std::map<int, Server*> _clientToServer;
    std::vector<int> _validListeningSockets;

    public:

      EPollManager(std::vector<Server>& servers);
      ~EPollManager();

      void run();
      void addSocketToEpoll(int fd);
      void acceptConnection(int serverFd);
      void handleClientRequest(int clientFd, Server *serv);
      std::vector<int> getValidListeningSockets() const;
      void determineDefaultServersAccordingToPort(std::vector<Server>& servers);

      void clean();
};

#endif