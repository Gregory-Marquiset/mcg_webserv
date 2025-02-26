
#include "../../includes/serverHandling/Server.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Server::Server(int domain, int service, int protocol, int port, u_long interface, int bcklg) {
    
    memset(this->_buffer, 0, (sizeof(this->_buffer)));
    
    this->_listeningSocket = new ListeningSocket(domain, service, protocol, port, interface, bcklg);

    /* j ajoute la socket serveur a la struct de control pollfd */
    pollfd newSocket;
    newSocket.fd = this->getListeningSocket()->getSockFd();
    newSocket.events = POLLIN; // POLLIN = des donnees en attente de lecture
    this->_allSockets.push_back(newSocket);
};

Server::~Server() {
    delete (this->_listeningSocket);
};

/* ================= GETTERS ======================== */

ListeningSocket* Server::getListeningSocket() {
    return (this->_listeningSocket);
}

char* Server::getBuffer() {
    return (this->_buffer);
}

std::vector<pollfd>& Server::getAllSockets() {
    return (this->_allSockets);
}

/* ================= CLIENT CONNECTION ======================== */

void Server::addClientToPollFdStruct(int fd) {
    
    pollfd newSocket;

    newSocket.fd = fd;
    newSocket.events = POLLIN;
    this->_allSockets.push_back(newSocket);
}

// (le accept() va dupliquer server socket -> du coup je pense que c est comme ca que poll() va savoir quels events concernent quel client)
// et surtout c est ce qui permet de laisser le socket serveur libre de toujours ecouter
// accept() cree une nouvelle socket cliente 
int Server::acceptConnection() {

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(this->getListeningSocket()->getSockFd(), (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        perror("Error: avec connect() fucntion...");
        exit(EXIT_FAILURE);
    }
    std::cout << "Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    return (newClientFd);
}

/* ================= GESTION DU SERVER ======================== */

// la methode poll() permet au serveur de rester a l ecoute de plusieurs sockets
// dans cette fonction, deux possibilites soit accepter une nouvelle connection, 
// soit gerer les evenements des sockets deja enregistrees

void Server::serverHandlingEvents(std::string root, std::string index) {
    
    if (this->_allSockets.empty()) {
        std::cout << "Error: aucune Socket active" << std::endl;
        return;
    }

    int res = poll(&_allSockets[0], this->_allSockets.size(), -1); 
    if (res < 0) {
        perror("poll error");
        return;
    }

    for (size_t i = 0; i < this->_allSockets.size(); ++i) {   
        if (this->_allSockets[i].revents & POLLIN) {
            if (this->_allSockets[i].fd == this->getListeningSocket()->getSockFd()) {
                std::cout << "il y a quelque chose a lire sur la socket server" << std::endl;
                int newFd = acceptConnection();
                addClientToPollFdStruct(newFd);
            }
            else {
                std::cout << "il y a quelque chose a lire sur une socket cliente" << std::endl;
                handleClientRequest(this->_allSockets[i].fd, root, index);
            }
        }
    }
}

/* Charles (et en attendant c est chat GPT) */

void Server::handleClientRequest(int clientFd, std::string root, std::string index) {
    
    std::string request;

    // Lire jusqu'à obtenir toute la requête (qui se termine par \r\n\r\n)
    while (1) {
        int bytesRead = recv(clientFd, this->_buffer, sizeof(this->_buffer) - 1, 0);

        if (bytesRead <= 0) {
            close(clientFd);
            std::cout << "bytes read <= 0 donc client déconnecté (FD " << clientFd << ")" << std::endl;
            return;
        }

        this->_buffer[bytesRead] = '\0';
        request += this->_buffer;
        std::cout << "requete = " << request << std::endl;

        // Vérifier si on a reçu toute la requête HTTP
        if (request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    responseFromServer(clientFd, request, root, index);
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
    
    std::cout << "----> " << filePath << std::endl;
    std::string content = getFileContent(filePath);

    if (content.empty()) {
        return "->>>>>>>>>>>>>>> HTTP/1.1 404 Not Found\r\n"
               "Content-Length: 13\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "404 Not  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa";
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

void Server::responseFromServer(int clientFd, const std::string& request, std::string root, std::string index) {
    std::istringstream requestStream(request);
    std::string method, path, httpVersion;
    requestStream >> method >> path >> httpVersion;

    std::cout << "Méthode : " << method << ", Chemin : " << path << std::endl;
    (void)root;
    // // (void)index;
    if (method == "GET") {
        std::string filePath = index;
        std::cout << "------> " << filePath << std::endl;
        std::string httpResponse = buildHttpResponse(filePath);
        send(clientFd, httpResponse.c_str(), httpResponse.size(), 0);
    } else {
        send(clientFd, "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found", 52, 0);
    }
}
