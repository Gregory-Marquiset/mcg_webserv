#include "../../includes/epollManager/EPollManager.hpp"
#include "../../includes/request/RequestParser.hpp"
#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

// epoll_create1(0) crée un file descriptor epoll.
// S'il échoue, un message d'erreur est affiché, et le programme quitte immédiatement.

EPollManager::EPollManager(std::vector<Server>& servers) : _servers(servers) {

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1) {
        throw (std::runtime_error("Error: epoll_create1() failed..."));
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
                        this->_validListeningSockets.push_back(fd);
                        addSocketToEpoll(fd);
                    }
                }
            } else {
                int port = it->first;
                for (size_t k = 0; k < servers[i].getListeningSocket().size(); ++k) {
                    if (port == servers[i].getListeningSocket()[k].getPort()) {
                        int fd = servers[i].getListeningSocket()[k].getSockFd();
                        close(fd);
                    }
                }
            }
        }
    }
    // for (size_t i = 0; i < this->getValidListeningSockets().size(); ++i) {
    //     std::cout << "registerered listening sockets: " << this->getValidListeningSockets()[i] << std::endl;
    // }
}

std::vector<int> EPollManager::getValidListeningSockets() const {
    return (this->_validListeningSockets);
}


void EPollManager::determineDefaultServersAccordingToPort(std::vector<Server>& servers) {

    std::vector<int> portAlreadyAssigned;

    for (size_t i = 0; i < servers.size(); ++i) {
        
        for (size_t j = 0; j < servers[i].getServerBlock().getPort().size(); ++j) {
        
            int portToCheck = servers[i].getServerBlock().getPort()[j];

            std::map<int, int> res;

            if (portAlreadyAssigned.empty()) {
                res[portToCheck] = 1;
                servers[i].addStatus(res);
                servers[i].setDefaultServer(1);

                portAlreadyAssigned.push_back(portToCheck);
            } else {
                int found = 0;
                for (size_t k = 0; k < portAlreadyAssigned.size(); ++k) {
                    if (portToCheck == portAlreadyAssigned[k]) {
                        found = 1;
                        break;
                    }
                }
                if (found == 1) {
                    res[portToCheck] = 0;
                } else {
                    res[portToCheck] = 1;
                    portAlreadyAssigned.push_back(portToCheck);
                    servers[i].setDefaultServer(1);
                }
                servers[i].addStatus(res);
            }
        }
    }

    // for (size_t i = 0; i < servers.size(); ++i) {
    //     std::cout << "--> " << servers[i].getDefaultServer() << std::endl;
    // }
}

EPollManager::~EPollManager() {

    if (this->_epollFd != -1) {
        close(_epollFd);
    }
}

/* ================= UTILS ======================== */

// Cette fonction ajoute une socket spécifique au système epoll
// pour surveiller les événements de lecture (EPOLLIN).
// Cet événement est configuré pour détecter les données disponibles en lecture (EPOLLIN).
// epoll_ctl() ajoute (EPOLL_CTL_ADD) la socket fd au descripteur epoll _epollFd

void EPollManager::addSocketToEpoll(int fd) {
    // std::cout << "FD = " << fd << std::endl; 

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw (std::runtime_error("Error: accept function failed..."));
    }
    // std::cout << "Socket " << fd << " added\n";
}


void EPollManager::acceptConnection(int serverFd) {
    // std::cout << "accept co: " << serverFd << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        throw (std::runtime_error("Error: accept function failed..."));
    }
    // std::cout << "SERVER: Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    
    // Mapping client fd to the correct server and socket
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
            if (serverFd == this->_servers[i].getListeningSocket()[j].getSockFd() && this->_servers[i].getDefaultServer() == 1) {
                this->_servers[i].getListeningSocket()[j].addClientToListeningSocket(newClientFd);

                std::cout << "accept co function: client fd nb " << newClientFd << " ajouté au server " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
                addSocketToEpoll(newClientFd);
                this->_clientToServer[newClientFd] = &this->_servers[i]; // où i est le server qui a accepté
                break;
            }
        }
    }
}

void EPollManager::clean() {
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        std::vector<ListeningSocket>& sockets = this->_servers[i].getListeningSocket();
        for (size_t j = 0; j < sockets.size(); ++j) {
            std::vector<int>& clients = sockets[j].getClientsFd();
            for (size_t k = 0; k < clients.size(); ++k) {
                close(clients[k]);
            }
            int sockFd = sockets[j].getSockFd();
            if (sockFd != -1) {
                close(sockFd);
            }
        }
    }
    close(this->_epollFd);
}

extern volatile sig_atomic_t exitFlag;

void EPollManager::run() {

    const int MAX_EVENTS = 100;
    this->_events.clear();
    this->_events.resize(MAX_EVENTS);

    while (exitFlag == 0) {
        int n = epoll_wait(this->_epollFd, this->_events.data(), MAX_EVENTS, -1);
        // if (n == -1) {
        //     throw (std::runtime_error("Error: epoll_wait() failed..."));
        // }
        if (n == -1) {
            if (errno == EINTR && exitFlag == 1) {
                // Interruption normale par signal → on sort
                break;
            }
            throw std::runtime_error("Error: epoll_wait() failed...");
        }

        for (int i = 0; i < n; ++i) {
            int fd = _events[i].data.fd; // le fd sur lequel event repere (c est le fd actif)

            // check si c est server
            int isServerSocket = 0;
            for (size_t j = 0; j < this->_servers.size(); j++) {
                for (size_t k = 0; k < this->_servers[j].getListeningSocket().size(); ++k) {
                    for (size_t l = 0; l < this->_validListeningSockets.size(); ++l) {
                        if  (fd == this->_validListeningSockets[l] && this->_servers[j].getDefaultServer() == 1) { 
                            // std::cout << "MAAAAAAAAAAATCH\n";
                            acceptConnection(fd);
                            // std::cout << "j ai bien accepte une connection sur le fd " << fd << std::endl;
                            isServerSocket = 1;
                            break;
                        }
                    }
                    if (isServerSocket == 1) {
                        break ;
                    }
                }
                if (isServerSocket == 1) {
                    break ;
                }
            }

            if (!isServerSocket) {
                
                int clientHandled = 0;

                for (size_t i = 0; i < this->_servers.size(); ++i) {
                    for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) { 

                        std::vector<int>& clients = this->_servers[i].getListeningSocket()[j].getClientsFd();
                        // std::cout << "client size: " << clients.size() << std::endl;
                        for (size_t k = 0; k < clients.size(); ++k) {
                            // std::cout << "clients[k] = " << clients[k] << std::endl;
                            if (fd == clients[k] && this->_servers[i].getDefaultServer() == 1) {
                                // Maintenant, utilise la map clientFd -> server pour retrouver le bon Server*
                                Server* realServer = this->_clientToServer[fd];
                                
                                if (realServer) {
                                    // std::cout << "Client trouvé sur fd = " << fd << std::endl;
                                    handleClientRequest(fd, realServer);
                                    clientHandled = 1;
                                    break;
                                }
                            }
                            
                            // std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
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




// void EPollManager::handleClientRequest(int clientFd, Server* serv) {
//     std::string buf;
//     std::string request;
//     ssize_t bytes_read;

//     buf.resize(BUFFER_SIZE);
//     std::cout << "+++++++++++++++++++++++++++++++++++++ Debut handleClientRequest" << std::endl;

//     while (request.find("\r\n\r\n") == std::string::npos) {
//         bytes_read = recv(clientFd, &buf[0], BUFFER_SIZE, 0);
        
//         std::cout << "clientFd: " << clientFd << std::endl;
//         if (bytes_read <= 0) {

//             if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
//                 perror("epoll_ctl: remove clientFd");
//             }
//             std::cerr << "Erreur handleClientRequest" << std::endl;
//             // FD_CLR(clientFd, this->_epollFd);
//             close(clientFd);
//             break;
//         }
//         request += buf;
//     }

//     ErrorManagement err;
//     RequestParser req_parser(err);

//     req_parser.parseRequest(request, clientFd);

//     if (err.getErrorCode() != 0) {
//         return;
//     }

//     ProcessRequest process_req(serv, req_parser, err);
//     ResponseMaker resp(err, process_req);
//     std::string response = resp.getFinalResponse();
//     size_t size = response.size();

//     std::cout << "Reponse: " << std::endl;
//     std::cout << response << std::endl;
//     send(clientFd, &response[0], size, 0);

//     if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
//         perror("epoll_ctl: remove clientFd");
//     }

//     close(clientFd);
//     std::cout << "+++++++++++++++++++++++++++++++++++++ Fin handleClientRequest" << std::endl;
// }




/*=====REECRITURE FONCTIONS PAR CHARLES=====*/
void	EPollManager::handleClientRequest(int clientFd, Server *serv)
{
    
    std::cout << " received server = " << &serv << std::endl;
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
			std::cerr << "erreur handleClientRequest" << std::endl;
			close(clientFd);
			break;
		}
		request += buf;
	}

	ErrorManagement	err;
	RequestParser	req_parser(err);

	req_parser.parseRequest(request, clientFd);
	
	if (err.getErrorCode() != 0)
	{
		return ;
	}

	ProcessRequest	process_req(serv, req_parser, err);

	ResponseMaker	resp(err, process_req);
	std::string		response = resp.getFinalResponse();
	size_t				size = response.size();

	std::cout << "Reponse :" << std::endl;
	std::cout << response << std::endl;
	send(clientFd, &response[0], size, 0);
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
        perror("epoll_ctl: remove clientFd");
    }
	close(clientFd);
}
