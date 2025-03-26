#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../WebServ.hpp"
#include "../server/Server.hpp"

// strace pour check les process

// epoll() en gros c est une structure de controle
// to poll = interroger/sonder -> donc interrogation/controle des fd (des sockets)

class EPollManager {
  private:
  std::vector<Server> _servers;     // Liste des serveurs
  int _epollFd;                      // Descripteur epoll
  std::vector<struct epoll_event> _events; // Liste des événements
  std::map<int, Server*> clientToServerMap; // pour savoir a quel server le client est associe

  public:

    EPollManager(const std::vector<Server>& servers);
    ~EPollManager();

    void run();
    void addSocketToEpoll(int fd);
    void acceptConnection(int serverFd);
    void handleClientRequest(int clientFd, Server *serv);
    void responseFromServer(int clientFd, std::string filePath);
};

#endif