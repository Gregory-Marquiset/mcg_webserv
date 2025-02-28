
/* ================= CLIENT CONNECTION ======================== */

void EPollManager::addClientToPollFdStruct(int fd) {
    
    pollfd newSocket;

    newSocket.fd = fd;
    newSocket.events = POLLIN;
    this->_allSockets.push_back(newSocket);
}

// (le accept() va dupliquer server socket -> du coup je pense que c est comme ca que poll() va savoir quels events concernent quel client)
// et surtout c est ce qui permet de laisser le socket serveur libre de toujours ecouter
// accept() cree une nouvelle socket cliente 
int EPollManager::acceptConnection() {

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(this->getListeningSocket()->getSockFd(), (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        perror("Error: avec connect() function...");
        exit(EXIT_FAILURE);
    }
    std::cout << "Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    return (newClientFd);
}

/* ================= GESTION DU SERVER ======================== */

// la methode poll() permet au serveur de rester a l ecoute de plusieurs sockets
// dans cette fonction, deux possibilites soit accepter une nouvelle connection, 
// soit gerer les evenements des sockets deja enregistrees

void EPollManager::serverHandlingEvents(std::string root, std::string index) {
    
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
