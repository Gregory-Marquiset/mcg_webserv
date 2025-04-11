#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "../WebServ.hpp"
#include "ASocket.hpp"

class ListeningSocket : public ASocket {

  private:  
    int _port;
    u_long _interface;
    int _backlog;
    int _listening;
    int _connection;

  public:
    ListeningSocket(int domain, int serice, int protocol, int port, u_long interface, int bklg);
    ~ListeningSocket();

    int getPort() const;
    
    int bindToNetwork(int sockFd, struct sockaddr_in address);
    void startListening();

    void printListeningSocketArgs(int domain, int service, int protocol, int port, u_long interface, int backlog);

};

#endif