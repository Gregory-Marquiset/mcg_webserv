#include "../../includes/server/Server.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Server::Server(const ServerBlock& serverBlock)
    : _serverBlock(serverBlock),
      _listeningSocket(AF_INET, SOCK_STREAM, 0, this->_serverBlock.getPort(), INADDR_ANY, 3)
{
    std::cout << "Initialisation du serveur " << this->_serverBlock.getHost().front().getHostName() << " sur le port " << this->_serverBlock.getPort() << std::endl;
}

Server::~Server() {};

/* ================= SETTERS ======================== */

void Server::setDefaultServer(int status) {
    this->_isDefaultServer = status;
}

/* ================= GETTERS ======================== */

int Server::getDefaultServer() const {
    return (this->_isDefaultServer);
}

ListeningSocket Server::getListeningSocket() const {
    return (this->_listeningSocket);
}
ServerBlock Server::getServerBlock() const {
    return (this->_serverBlock);
}

std::vector<Server> Server::getAllServers(const std::vector<ServerBlock>& serverBlocks) {
    std::vector<Server> servers;

    for (std::vector<ServerBlock>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {

        servers.push_back(Server(*it));
    }
    return (servers);
}

void Server::printServerInfo() const {
    std::cout << "===== Server Info =====" << std::endl;
    std::cout << "Server Socket: " << this->_listeningSocket.getSockFd() << std::endl;
    std::cout << "Port: " << this->_serverBlock.getPort() << std::endl;
    std::cout << "Root: " << this->_serverBlock.getRoot() << std::endl;
    std::cout << "Index: " << this->_serverBlock.getIndex() << std::endl;
    std::cout << "Body size: " << this->_serverBlock.getClientMaxBodySize() << std::endl;

    for (size_t i = 0; i < this->_serverBlock.getAllowMethods().size(); ++i) {
        std::cout << "Allow method: " << this->_serverBlock.getAllowMethods()[i] << std::endl;
    }

    for (size_t i = 0; i < this->_serverBlock.getCgiExtension().size(); ++i) {
        std::cout << "CGI extension: key -> " << this->_serverBlock.getCgiExtension()[i].getKey() << " value -> " << this->_serverBlock.getCgiExtension()[i].getValue() << std::endl;
    }

    for (size_t i = 0; i < this->_serverBlock.getLocationBlock().size(); ++i) {
        std::cout << "Location Path: " << this->_serverBlock.getLocationBlock()[i].getPath() << std::endl;
        std::cout << "  Root: " << this->_serverBlock.getLocationBlock()[i].getRoot() << std::endl;
        std::cout << "  Index: " << this->_serverBlock.getLocationBlock()[i].getIndex() << std::endl;
        std::cout << "  Client Max Body: " << this->_serverBlock.getLocationBlock()[i].getClientMaxBodySize() << std::endl;

        for (size_t j = 0; j < this->_serverBlock.getLocationBlock()[i].getAllowMethods().size(); ++j) {
            std::cout << "  Allow method: " << this->_serverBlock.getLocationBlock()[i].getAllowMethods()[j] << std::endl;
        }

        for (size_t j = 0; j < this->_serverBlock.getLocationBlock()[i].getCgiExtension().size(); ++j) {
            std::cout << "  CGI extension: key -> " << this->_serverBlock.getLocationBlock()[i].getCgiExtension()[j].getKey() << " value -> " << this->_serverBlock.getLocationBlock()[i].getCgiExtension()[j].getValue() << std::endl;        }
    }
    std::cout << "=======================" << std::endl;
}
