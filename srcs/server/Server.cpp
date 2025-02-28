#include "../../includes/server/Server.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Server::Server(int domain, int service, int protocol, u_long interface, int bcklg) {
    
    memset(this->_buffer, 0, (sizeof(this->_buffer)));

    this->_readyBlock = new ReadyBlock();

    int port = getListen(); // il va falloir que je reussisse a choper le bon block 
    
    this->_listeningSocket = new ListeningSocket(domain, service, protocol, port, interface, bcklg);

    
    /* j ajoute la socket serveur a la struct de control pollfd */ // -> ca va devoir passer dans EPollManager
    pollfd newSocket;
    newSocket.fd = this->getListeningSocket()->getSockFd();
    newSocket.events = POLLIN; // POLLIN = des donnees en attente de lecture
    this->_allSockets.push_back(newSocket);
};

Server::~Server() {
    delete (this->_listeningSocket);
    delete (this->_readyBlock);
};

/* ================= GETTERS ======================== */

ListeningSocket* Server::getListeningSocket() const {
    return (this->_listeningSocket);
}

char* Server::getBuffer() {
    return (this->_buffer);
}

ReadyBlock* ReadyBlock::getReadyBlock() const {
    return (this->_readyBlock);
}


