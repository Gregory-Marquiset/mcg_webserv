#ifndef SERVER_HPP
#define SERVER_HPP

#include "../WebServ.hpp"
#include "ListeningSocket.hpp"
#include "../configFile/ReadyBlock.hpp"

class Server {

  private:
    ListeningSocket* _listeningSocket;
    ReadyBlock* _readyBlock;
    char _buffer[1024];

  public:
    Server(int domain, int service, int protocol, u_long interface, int bcklg);
    ~Server();

    /* getters */

    ListeningSocket *getListeningSocket();
    ReadyBlock *getReadyBlock();
};

#endif