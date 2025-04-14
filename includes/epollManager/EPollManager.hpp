#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../WebServ.hpp"
#include "../server/Server.hpp"

// strace pour check les process

// epoll() en gros c est une structure de controle
// to poll = interroger/sonder -> donc interrogation/controle des fd (des sockets)

class EPollManager {
  private:
  std::vector<Server>& _servers;     // Liste des serveurs
  int _epollFd;                      // Descripteur epoll
  std::vector<struct epoll_event> _events; // Liste des événements
  std::vector<int> _validListeningSockets;
  // std::vector<int> _listOfClients;
  // std::map<int, Server*> _listeningFdToServer;
  // std::map<int, Server*> _clientFdToServer;

  public:

    EPollManager(std::vector<Server>& servers);
    ~EPollManager();

    void determineDefaultServersAccordingToPort(std::vector<Server>& servers);

    void run();
    void addSocketToEpoll(int fd);
    // void addValidListeningSockets(std:vector<int> fd);
    std::vector<int> getValidListeningSockets() const;
    void acceptConnection(int serverFd);
    void handleClientRequest(int clientFd, Server& serv);
    void responseFromServer(int clientFd, std::string filePath);
};

#endif