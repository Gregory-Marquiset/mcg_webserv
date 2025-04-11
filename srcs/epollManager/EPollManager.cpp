#include "../../includes/epollManager/EPollManager.hpp"
#include "../../includes/request/RequestParser.hpp"
#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

EPollManager::EPollManager(std::vector<Server>& servers) : _servers(servers) {

    this->_epollFd = epoll_create1(0); // c est le fd monitor
    std::cout << "epoll manager fd = " << this->_epollFd << std::endl;
    if (this->_epollFd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    determineDefaultServersAccordingToPort(this->_servers);

    // std::vector<Server>::iterator it;
    // std::vector<ListeningSocket>::iterator iSock;
    // std::cout << "server count = " << this->_servers.size() << std::endl;
    // for (it = this->_servers.begin(); it != this->_servers.end(); ++it) {

    //     for (iSock = it->getListeningSocket().begin(); iSock != it->getListeningSocket().end(); ++iSock) {

    //         std::cout << "-> " << iSock->getSockFd() << std::endl;
    //         std::cout << "Trying to add fd: " << iSock->getSockFd() << std::endl;
    //         // if (it->getServerStatusAccordingToPort().second == 1) {
    //         //     std::cout << "1\n";
    //             addSocketToEpoll(iSock->getSockFd());
    //     }
    // }
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        std::cout << "nb de listenng socket " << this->_servers[i].getListeningSocket().size() << std::endl;
        for (size_t iSock = 0; iSock < this->_servers[i].getListeningSocket().size(); ++iSock) {
            std::cout << "-> " << this->_servers[i].getListeningSocket()[iSock].getSockFd() << std::endl;
            std::cout << "Trying to add fd: " << this->_servers[i].getListeningSocket()[iSock].getSockFd() << std::endl;
            
            std::map<int, int> status = this->_servers[i].getServerStatusAccordingToPort();
            for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
                std::cout << "Server " << i << " Is default server for port: " << it->first << " = " << it->second << std::endl;
                if ((this->_servers[i].getListeningSocket()[iSock].getPort() == it->first) && (it->second == 1)) {
                    std::cout << "ici\n";
                    addSocketToEpoll(this->_servers[i].getListeningSocket()[iSock].getSockFd());
                }
            }
        
            
            // if (this->_servers[i].getServerStatusAccordingToPort().second == 1) {
            //     std::cout << "1\n";
            //     addSocketToEpoll(this->_servers[i].getListeningSocket()[iSock].getSockFd());
            // }
        }
    }
}

    // for (size_t i = 0; i < this->_servers.size(); ++i) {

    //     // std::cout << "size = " << servers[i].getServerStatusAccordingToPort().size() << std::endl;
    //     std::map<int, int> status = this->_servers[i].getServerStatusAccordingToPort();
    //     for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
    //         std::cout << "Server " << i << " Is default server for port: " << it->first << " = " << it->second << std::endl;
    //         if (it->second == 1)
    //                 addSocketToEpoll(this->_servers[i].getListeningSocket()[iSock].getSockFd());

    //     }
    // }

    
void EPollManager::determineDefaultServersAccordingToPort(std::vector<Server>& servers) {

    std::vector<int> portAlreadyAssigned;

    for (size_t i = 0; i < servers.size(); ++i) {
        
        std::cout << "------- entre server: " << i << "-------" << std::endl;
        for (size_t j = 0; j < servers[i].getServerBlock().getPort().size(); ++j) {
        
            int portToCheck = servers[i].getServerBlock().getPort()[j];
            std::cout << "port to check = " << portToCheck << std::endl;

            std::map<int, int> res;

            if (portAlreadyAssigned.empty()) {
                res[portToCheck] = 1;
                servers[i].addStatus(res);
                portAlreadyAssigned.push_back(portToCheck);
            } else {

                for (size_t k = 0; k < portAlreadyAssigned.size(); ++k) {
                    if (portToCheck == portAlreadyAssigned[k]) {
                        std::cout << "doublon: " << "port already assigned " << portAlreadyAssigned[k] << std::endl;
                        res[portToCheck] = 0;
                        servers[i].addStatus(res);
                        break ;
                    } 
                }
                res[portToCheck] = 1;

                std::cout << "port " << res.begin()->first << std::endl;
                std::cout << "status " << res[portToCheck] << std::endl;

                servers[i].addStatus(res);
                portAlreadyAssigned.push_back(portToCheck);
            }
        }
        std::map<int, int> status = servers[i].getServerStatusAccordingToPort();
        for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
            std::cout << "Is default server for port: " << it->first << " = " << it->second << std::endl;
        }

        std::cout << "CHECK " << servers[i].getServerStatusAccordingToPort().size() << std::endl;
    }
}
    

    
    /* ================= UTILS ======================== */
    
    // Cette fonction ajoute une socket spécifique au système epoll
    // pour surveiller les événements de lecture (EPOLLIN).
    // Cet événement est configuré pour détecter les données disponibles en lecture (EPOLLIN).
    // epoll_ctl() ajoute (EPOLL_CTL_ADD) la socket fd au descripteur epoll _epollFd

    
void EPollManager::addSocketToEpoll(int fd) {
    std::cout << "FD = " << fd << std::endl; 

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl: ajout socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket " << fd << " added\n";
}


EPollManager::~EPollManager() {

    close(_epollFd);
}

/* ================= UTILS ======================== */

// epoll_wait() bloque jusqu'à ce qu'un événement survienne. Il remplit _events avec les descripteurs actifs. En cas d'erreur, le programme s'arrête.
void EPollManager::run() {

    const int MAX_EVENTS = 10; // le nombre maximal d'événements à traiter simultanément
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
                for (size_t k = 0; k < this->_servers[j].getListeningSocket().size(); ++k) {
                    if (fd == _servers[j].getListeningSocket()[k].getSockFd()) { // changer le .front() faire en sorte d avoir la bonne listening socket
                        acceptConnection(fd);
                        isServerSocket = 1;
                        break;
                    }
                }
            }

            // client donc handle requête
            if (!isServerSocket) {
                if (clientToServerMap.find(fd) != clientToServerMap.end()) {
                    Server* server = clientToServerMap[fd]; // Récupérer le serveur correspondant
                    handleClientRequest(fd, server);
                } else {
                    std::cerr << "Error: le serveur fd: " << fd << " est introuvable." << std::endl;
                    close(fd);
                }
            }
        }

    }
}

// Initialisation (EPollManager)

//     Crée epoll.
//     Ajoute les sockets des serveurs à epoll.

// Boucle principale (run())

//     Attend des événements avec epoll_wait().
//     Si un serveur est détecté : accepter une connexion (acceptConnection()).
//     Si un client est détecté : traiter sa requête (handleClientRequest()).

// Gestion des connexions (acceptConnection())

//     Accepte le client via accept().
//     Associe la socket client au bon serveur.
//     Ajoute la socket client à epoll.


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
        for (size_t k = 0; k < this->_servers[j].getListeningSocket().size(); ++k) {

            if (serverFd == _servers[j].getListeningSocket()[k].getSockFd()) {
                clientToServerMap[newClientFd] = &_servers[j];
                break;
            }
        }
    }
    addSocketToEpoll(newClientFd);
}

/*=====REECRITURE FONCTIONS PAR CHARLES=====*/
void	EPollManager::handleClientRequest(int clientFd, Server *serv)
{
	std::string		buf;
	std::string		request;
	ssize_t			bytes_read;

	buf.resize(BUFFER_SIZE);
	std::cout << std::endl << std::endl << std::endl;
	std::cout << "Debut handleClientRequest" << std::endl;
	std::cout << "clientFd :" << clientFd << std::endl;
	while (request.find("\r\n\r\n") == std::string::npos)
	{
		buf.resize(BUFFER_SIZE);
		bytes_read = recv(clientFd, &buf[0], BUFFER_SIZE, 0);
		std::cout << buf << std::endl;
		if (bytes_read <= 0)
		{
			// close(clientFd);
			std::cerr << "erreur handleClientRequest" << std::endl;
			close(clientFd);
			break;
		}
		request += buf;
	}

	ErrorManagement	err;
	RequestParser	req_parser(err);

	req_parser.parseRequest(request, clientFd);
	// std::cout << "Elem de la requete apres le parse :" << std::endl;
	// std::cout << "Method : " << req_parser.getMethod() << std::endl;
	// std::cout << "URI : " << req_parser.getURI() << std::endl;
	// std::cout << "HTTP version : " << req_parser.getHTTP() << std::endl;
	// std::cout << std::endl;
	if (err.getErrorCode() != 0)
	{
		return ;
	}

	ProcessRequest	process_req(serv, req_parser, err);
	if (req_parser.getIsCgi() == true)
	{
		// GREG TU TE DEMERDES POUR LA REP
	}

	ResponseMaker	resp(err, process_req);
	std::string		response = resp.getFinalResponse();
	size_t				size = response.size();

	std::cout << "Reponse :" << std::endl;
	std::cout << response << std::endl;
	send(clientFd, &response[0], size, 0);
}
