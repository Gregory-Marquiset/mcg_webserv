#include "../../includes/epollManager/EPollManager.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

EPollManager::EPollManager(const std::vector<Server>& servers) {

    this->_servers = servers;

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    std::vector<Server>::iterator it;
    for (it = this->_servers.begin(); it != this->_servers.end(); ++it) {
        addSocketToEpoll(it->getListeningSocket().getSockFd());
    }
}

EPollManager::~EPollManager() {

    close(_epollFd);
}

/* ================= UTILS ======================== */

void EPollManager::addSocketToEpoll(int fd) {
    
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl: ajout socket");
        exit(EXIT_FAILURE);
    }
}

void EPollManager::run() {

    const int MAX_EVENTS = 10;
    this->_events.resize(MAX_EVENTS);

    while (1) {
        int n = epoll_wait(_epollFd, _events.data(), MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i) {
            int fd = _events[i].data.fd; // le fd sur lequel event repere
        
            // check si c est server
            int isServerSocket = 0;
            for (size_t j = 0; j < _servers.size(); j++) {
                if (fd == _servers[j].getListeningSocket().getSockFd()) {
                    acceptConnection(fd);
                    isServerSocket = 1;
                    break;
                }
            }
        
            // client donc handle requête
            if (!isServerSocket) {
                if (clientToServerMap.find(fd) != clientToServerMap.end()) {
                    Server* server = clientToServerMap[fd]; // Récupérer le serveur correspondant
                    handleClientRequest(fd, server->getServerBlock().getRoot(), server->getServerBlock().getIndex());
                } else {
                    std::cerr << "Error: le serveur fd: " << fd << " est introuvable." << std::endl;
                    close(fd);
                }
            }
        }
        
    }
}

// (le accept() va dupliquer server socket
// et c est ce qui permet de laisser le socket serveur libre de toujours ecouter
// accept() cree une nouvelle socket cliente 
void EPollManager::acceptConnection(int serverFd) {

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        perror("Error: avec connect() fucntion...");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER: Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    
    // pour associer la requete cliente au bon server
    for (size_t j = 0; j < _servers.size(); j++) {
        if (serverFd == _servers[j].getListeningSocket().getSockFd()) {
            clientToServerMap[newClientFd] = &_servers[j];
            break;
        }
    }
    addSocketToEpoll(newClientFd);
}

/* tout ca en dessous, c est pas moi, c etait pour pouvoir check les requetes http au lieu de curl ou telnet */
/* donc tout reprendre */

void EPollManager::handleClientRequest(int clientFd, std::string root, std::string index) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        close(clientFd);
        return;
    }

    buffer[bytesRead] = '\0';
    std::cout << "[Client] Reçu : " << buffer << std::endl;

    std::istringstream requestStream(buffer);
    std::string method, path, httpVersion;
    requestStream >> method >> path >> httpVersion;

    if (method == "GET") {
        (void)root;
        std::string filePath = index;
        responseFromServer(clientFd, filePath);
    } else {
        send(clientFd, "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n", 54, 0);
    }
}

std::string getFileContent(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return ""; // Fichier introuvable
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string buildHttpResponse(const std::string& filePath) {
    std::string content = getFileContent(filePath);

    if (content.empty()) {
        return "HTTP/1.1 404 Not Found\r\n"
               "Content-Length: 13\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "404 Not Found";
    }

    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Length: " << content.size() << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << content;

    return response.str();
}

void EPollManager::responseFromServer(int clientFd, std::string filePath) {
    std::cout << "--------------> " << filePath << std::endl;

    std::string httpResponse = buildHttpResponse(filePath);
    send(clientFd, httpResponse.c_str(), httpResponse.size(), 0);
}
