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

    for (size_t i = 0; i < servers.size(); ++i) {
        std::map<int, int> status = servers[i].getServerStatusAccordingToPort();
        for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
            if (it->second == 1) {
                int port = it->first;
                for (size_t k = 0; k < servers[i].getListeningSocket().size(); ++k) {
                    if (port == servers[i].getListeningSocket()[k].getPort()) {
                        int fd = servers[i].getListeningSocket()[k].getSockFd();
                        addSocketToEpoll(fd);
                        this->_validListeningSockets.push_back(fd);
                    }
                }
            }
        }
    }
}

std::vector<int> EPollManager::getValidListeningSockets() const {
    return (this->_validListeningSockets);
}

// void EPollManager::addValidListeningSockets(std::vector<int>)

void EPollManager::determineDefaultServersAccordingToPort(std::vector<Server>& servers) {

    std::vector<int> portAlreadyAssigned;

    for (size_t i = 0; i < servers.size(); ++i) {
        
        // std::cout << "------- entre server: " << i << "-------" << std::endl;
        for (size_t j = 0; j < servers[i].getServerBlock().getPort().size(); ++j) {
        
            int portToCheck = servers[i].getServerBlock().getPort()[j];
            // std::cout << "port to check = " << portToCheck << std::endl;

            std::map<int, int> res;

            if (portAlreadyAssigned.empty()) {
                res[portToCheck] = 1;
                servers[i].addStatus(res);
                portAlreadyAssigned.push_back(portToCheck);
            } else {

                for (size_t k = 0; k < portAlreadyAssigned.size(); ++k) {
                    if (portToCheck == portAlreadyAssigned[k]) {
                        // std::cout << "doublon: " << "port already assigned " << portAlreadyAssigned[k] << std::endl;
                        res[portToCheck] = 0;
                        servers[i].addStatus(res);
                        break ;
                    } 
                }
                res[portToCheck] = 1;

                // std::cout << "port " << res.begin()->first << std::endl;
                // std::cout << "status " << res[portToCheck] << std::endl;

                servers[i].addStatus(res);
                portAlreadyAssigned.push_back(portToCheck);
            }
        }
        // std::map<int, int> status = servers[i].getServerStatusAccordingToPort();
        // for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
        //     std::cout << "Is default server for port: " << it->first << " = " << it->second << std::endl;
        // }
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

// je boucle sur les servers
// chaque server a un vecteur de listning sockets
// chaque sockets a un port et un fd
// chaque server a un vecteur de connecting sockets

// fd c est le fd sur lequel l event est repere
// il faut que je check pour vois si fd corresponds a une listeningSocket->getFd()

// int EPollManager::isValidSocket(std::vector<Server&> servers) {
//     for (size_t i = 0; i < servers.size(); ++i) {

//         for (size_t j = 0; j < this->_validListeningSockets.size(); ++j) {
//             if (servers[i].getListeningSocket)
//         }
//     }
// }

// epoll_wait() bloque jusqu'à ce qu'un événement survienne. Il remplit _events avec les descripteurs actifs. En cas d'erreur, le programme s'arrête.

// void EPollManager::run() {

// }

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
            int fd = _events[i].data.fd; // le fd sur lequel event repere (c est le fd actif)

            // check si c est server
            int isServerSocket = 0;
            for (size_t j = 0; j < this->_servers.size(); j++) {
                
                for (size_t k = 0; k < this->_servers[j].getListeningSocket().size(); ++k) {
                    // std::cout << "--> fd = " << fd << std::endl;
                    // std::cout << "--> comp = " << this->_servers[j].getListeningSocket()[k].getSockFd() << std::endl;

                    for (size_t l = 0; l < this->_validListeningSockets.size(); ++l) {

                        if  (fd == this->_validListeningSockets[l]) {
                            // if (fd == this->_servers[j].getListeningSocket()[k].getSockFd()) { // donc la pb
                            std::cout << "send " << fd << " to accept co\n";
                            acceptConnection(fd); // PROBLEM DU FD TRANSMIS ICI
                            std::cout << "j ai bien accepte une connection sur le fd " << fd << std::endl; // des fois j accepte la mauvaise co
                            
                            isServerSocket = 1;
                            break;
                        }
                    }
                    if (isServerSocket == 1) {
                        std::cout << "check\n";
                        break ;
                    }
                    
                }
                if (isServerSocket == 1) {
                    std::cout << "check\n";
                    break ;
                }
            }

            if (!isServerSocket) {
                int clientHandled = 0;
            
                for (size_t i = 0; i < this->_servers.size(); ++i) {

                   
                    for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
                        
                        if (this->_servers[i].getListeningSocket()[j].getClientsFd().empty()) {
                        std::cout << "empty\n";
                        }

                     
                        std::vector<int>& clients = this->_servers[i].getListeningSocket()[j].getClientsFd();

                        std::cout << "size = " << clients.size() << std::endl;
                        for (size_t k = 0; k < clients.size(); ++k) {
                            if (fd == clients[k]) {
                                std::cout << "Client trouvé sur fd = " << fd << std::endl;
                                handleClientRequest(fd, this->_servers[i]);
                                clientHandled = 1;
                                break;
                            }

                         
                        }
                        if (clientHandled == 1) 
                            break;
                    }
                    if (clientHandled == 1) 
                        break;
                }
            
                if (clientHandled == 0) {
                    std::cerr << "Erreur: fd client " << fd << " non trouvé dans aucune socket serveur. Fermeture." << std::endl;
                    close(fd);
                }
            }
            
        }
    }
}

 // client donc handle requête
            // if (!isServerSocket) {
            //     std::cout << "coucou\n";
            //     for (size_t i = 0; i < this->_servers.size(); ++i) {
            //         for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
            //             // std::cout << "ici\n";
            //             // std::cout << this->_servers[i].getListeningSocket()[j].getClientsFd().size() << std::endl;

            //             std::cout << "fd client -> " << fd << std::endl;
            //             std::cout << "server fd = " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
            //             if (this->_servers[i].getListeningSocket()[j].getClientsFd().empty()) {
            //                 if (this->_servers[i].getListeningSocket()[j].getClientsFd()[z] == this->_servers[i].getListeningSocket()[j].getSockFd()) {
            //                     std::cout << "coucou\n";
            //                     handleClientRequest(fd, this->_servers[i]);
            //                 } else {
            //                     // std::cerr << "1 Error: le serveur fd: " << fd << " est introuvable." << std::endl;
            //                     // close(fd);
            //                     std::cout << "1\n";
            //                 }
            //             }
            //             for (size_t k = 0; k < this->_servers[i].getListeningSocket()[j].getClientsFd().size(); ++k) {

            //                 std::cout << "fd -> " << fd << std::endl;
            //                 std::cout << "cleint fd = " << this->_servers[i].getListeningSocket()[j].getClientsFd()[k] << std::endl;
    
            //                 if (fd == this->_servers[i].getListeningSocket()[j].getClientsFd()[k]) {
            //                     std::cout << "coucou\n";
            //                     handleClientRequest(fd, this->_servers[i]);
            //                 } else {
            //                     std::cerr << "2 Error: le serveur fd: " << fd << " est introuvable." << std::endl;
            //                     close(fd);
            //                 }
            //             }
            //         }
            //     }
            // }

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

    std::cout << "accept co: " << serverFd << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        perror("Error: avec connect() fucntion...");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER: Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
            // std::cout << "server fd = " << serverFd << std::endl;
            // std::cout <<  "server fd compared with = " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
            if (serverFd == this->_servers[i].getListeningSocket()[j].getSockFd()) {
                this->_servers[i].getListeningSocket()[j].addClientToListeningSocket(newClientFd);
                std::cout << "client fd nb " << newClientFd << " ajoute au server " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
                addSocketToEpoll(newClientFd);
                break ;
            }
        }
    }
}

/*=====REECRITURE FONCTIONS PAR CHARLES=====*/
void	EPollManager::handleClientRequest(int clientFd, Server& serv)
{
	std::string		buf;
	std::string		request;
	ssize_t			bytes_read;

	buf.resize(BUFFER_SIZE);
	std::cout << std::endl << std::endl << std::endl;
	std::cout << "+++++++++++++++++++++++++++++++++++++ Debut handleClientRequest" << std::endl;
	std::cout << "clientFd :" << clientFd << std::endl;
	while (request.find("\r\n\r\n") == std::string::npos)
	{
		// buf.resize(BUFFER_SIZE);
        std::cout << "clientFd :" << clientFd << std::endl;
        std::cout << &buf[0] << std::endl;
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

	ProcessRequest	process_req(&serv, req_parser, err);
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
    std::cout << "+++++++++++++++++++++++++++++++++++++ Fin handleClientRequest" << std::endl;

}
