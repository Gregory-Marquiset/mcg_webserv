#include "../../includes/server/Server.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Server::Server(const ServerBlock& serverBlock) {

    this->_serverBlock = serverBlock;
    for (size_t i = 0; i < this->_serverBlock.getPort().size(); ++i) {
        this->_listeningSocket.push_back(ListeningSocket(AF_INET, SOCK_STREAM, 0, this->_serverBlock.getPort()[i], INADDR_ANY, 3));
    }
}

Server::~Server() {};

/* ================= SETTERS ======================== */

void Server::setDefaultServer(int status) {
    this->_isDefaultServer = status;
}

/* ================= GETTERS ======================== */

std::map<int, int> Server::getServerStatusAccordingToPort() const {
    return (this->_serverStatusAccordingToPort);
}

int Server::getDefaultServer() const {
    return (this->_isDefaultServer);
}

std::vector<ListeningSocket>& Server::getListeningSocket() {
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

void Server::addStatus(const std::map<int, int>& status) {
    this->_serverStatusAccordingToPort.insert(status.begin(), status.end());
}

// // en faite non faire ca a partir de la listening socket
// void Server::addClientToServerList(int newClientFd) {
//     this->_clientsFd.push_back(newClientFd);
//     std::cout << "client fd nb " << newClientFd << " added au server " << this->getListeningSocket().getSockFd() << std::endl;
// }

void Server::printServerInfo() const {
    std::cout << "===== Server Info =====" << std::endl;
    
    for (size_t i = 0; i < this->_listeningSocket.size(); ++i) {
        std::cout << "Server Socket: " << this->_listeningSocket[i].getSockFd() << std::endl;
    }    
    for (size_t i = 0; i < this->_serverBlock.getPort().size(); ++i) {
        std::cout << "Port: " << this->_serverBlock.getPort()[i] << std::endl;
    }    
    
    std::cout << "Root: " << this->_serverBlock.getRoot() << std::endl;
    std::cout << "Index: " << this->_serverBlock.getIndex() << std::endl;
    std::cout << "Body size: " << this->_serverBlock.getClientMaxBodySize() << std::endl;
    std::cout << "Auto index " << this->_serverBlock.getAutoIndex() << std::endl;

    if (this->_serverBlock.getRedirection().empty())
        std::cout << "Return: false (NULL)" << std::endl;
    else 
        std::cout << "Return: " << this->_serverBlock.getRedirection().front() << " " << this->_serverBlock.getRedirection()[1] <<std::endl;

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
        std::cout << "  Autoindex: " << this->_serverBlock.getLocationBlock()[i].getAutoIndex() << std::endl;

        if (this->_serverBlock.getLocationBlock()[i].getRedirection().empty())
            std::cout << "  Return: false (NULL)" << std::endl;
        else 
            std::cout << "  Return: " << this->_serverBlock.getLocationBlock()[i].getRedirection().front() << " " << this->_serverBlock.getLocationBlock()[i].getRedirection()[1] <<std::endl;
        
        for (size_t j = 0; j < this->_serverBlock.getLocationBlock()[i].getAllowMethods().size(); ++j) {
            std::cout << "  Allow method: " << this->_serverBlock.getLocationBlock()[i].getAllowMethods()[j] << std::endl;
        }

        for (size_t j = 0; j < this->_serverBlock.getLocationBlock()[i].getCgiExtension().size(); ++j) {
            std::cout << "  CGI extension: key -> " << this->_serverBlock.getLocationBlock()[i].getCgiExtension()[j].getKey() << " value -> " << this->_serverBlock.getLocationBlock()[i].getCgiExtension()[j].getValue() << std::endl;        
        }
    }
    std::cout << "=======================" << std::endl;
}
