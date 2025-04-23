#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../WebServ.hpp"
#include "../server/Server.hpp"

class EPollManager {
  private:
    std::vector<Server>& _servers;
    int _epollFd;
    std::vector<struct epoll_event> _events;
    std::vector<int> _validListeningSockets;
    std::map<int, Server*> _getServerBasedOnClientFd; // la je regarde le fd du client retourne par accept() et je cherhcer quel server a accepter le client (cle = fd client, value: Server*)
    std::map<int, Server*> _getServerBasedOnServerFd; // la je regarde le fd server donc je cherche un server (cle = serverFd, value: Server*)
    std::map<int, ListeningSocket*> _getListeningSocketBasedOnServerFd; // la je cherche la listening socket de mon server (cle = fd server, value: ListeningSocket*)

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