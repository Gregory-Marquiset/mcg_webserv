#include "../../includes/server/Server.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Server::Server(const ServerBlock& serverBlock)
    : _serverBlock(serverBlock),
      _listeningSocket(AF_INET, SOCK_STREAM, 0, _serverBlock.getListen(), INADDR_ANY, 3)
{
    std::cout << "Initialisation du serveur " << _serverBlock.getServerName() << " sur le port " << _serverBlock.getListen() << std::endl;
}

Server::~Server() {};

/* ================= GETTERS ======================== */

ListeningSocket Server::getListeningSocket() const {
    return (this->_listeningSocket);
}

// je sais plus ou je dois mettre ca 
// char* Server::getBuffer() {
//     return (this->_buffer);
// }

ServerBlock Server::getServerBlock() const {
    return (this->_serverBlock);
}

std::vector<Server> Server::getAllServers(const std::vector<ServerBlock>& serverBlocks) {
    std::vector<Server> servers;

    for (std::vector<ServerBlock>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
        servers.push_back(Server(*it)); 
    }

    return servers;
}

void Server::printServerInfo() const {
    std::cout << "===== Server Info =====" << std::endl;
    std::cout << "Server Socket: " << this->_listeningSocket.getSockFd() << std::endl;
    std::cout << "Server Name: " << this->_serverBlock.getServerName() << std::endl;
    std::cout << "Port: " << this->_serverBlock.getListen() << std::endl;
    std::cout << "Root: " << this->_serverBlock.getRoot() << std::endl;
    std::cout << "Index: " << this->_serverBlock.getIndex() << std::endl;
    std::cout << "=======================\n" << std::endl;
}