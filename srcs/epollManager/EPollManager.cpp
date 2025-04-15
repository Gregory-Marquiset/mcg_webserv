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
                        this->_validListeningSockets.push_back(fd);
                        addSocketToEpoll(fd);
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < this->getValidListeningSockets().size(); ++i) {
        std::cout << "registerered lis sockets: " << this->getValidListeningSockets()[i] << std::endl;
    }
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
                }
                servers[i].addStatus(res);
            }
        }
    }
}
    
/* ================= UTILS ======================== */

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

    close(this->_epollFd);
}

/* ================= UTILS ======================== */

void EPollManager::run() {

    const int MAX_EVENTS = 100; // le nombre maximal d'événements à traiter simultanément
    this->_events.clear();
    this->_events.resize(MAX_EVENTS);

    while (1) {
        int n = epoll_wait(this->_epollFd, this->_events.data(), MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            std::cerr << "errno = " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i) {
            int fd = _events[i].data.fd; // le fd sur lequel event repere (c est le fd actif)

            // check si c est server
            int isServerSocket = 0;
            for (size_t j = 0; j < this->_servers.size(); j++) {
                for (size_t k = 0; k < this->_servers[j].getListeningSocket().size(); ++k) {
                    for (size_t l = 0; l < this->_validListeningSockets.size(); ++l) {
                        if  (fd == this->_validListeningSockets[l]) { 
                            std::cout << "MAAAAAAAAAAATCH\n";
                            acceptConnection(fd);
                            std::cout << "j ai bien accepte une connection sur le fd " << fd << std::endl;
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
                        for (size_t k = 0; k < clients.size(); ++k) {
                            std::cout << "clients[k] = " << clients[k] << std::endl;
                            if (fd == clients[k]) {
                                std::cout << "Client trouvé sur fd = " << fd << std::endl;
                                handleClientRequest(fd, this->_servers[i]);
                                clientHandled = 1;
                                break;
                            }
                            std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
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

            

              


// (le accept() va dupliquer server socket
// et c est ce qui permet de laisser le socket serveur libre de toujours ecouter
// accept() cree une nouvelle socket cliente
// void EPollManager::acceptConnection(int serverFd) {

//     std::cout << "accept co: " << serverFd << std::endl;
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);

//     int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
//     if (newClientFd < 0) {
//         perror("Error: avec connect() fucntion...");
//         exit(EXIT_FAILURE);
//     }
//     std::cout << "SERVER: Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    
//     for (size_t i = 0; i < this->_servers.size(); ++i) {
//         for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
//             if (serverFd == this->_servers[i].getListeningSocket()[j].getSockFd()) {
//                 this->_servers[i].getListeningSocket()[j].addClientToListeningSocket(newClientFd);
//                 _fdToListeningSocket[newClientFd] = &this->_servers[i].getListeningSocket()[j];
//                 _fdToServer[newClientFd] = &this->_servers[i];

//                 std::cout << "client fd nb " << newClientFd << " ajoute au server " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
//                 addSocketToEpoll(newClientFd);
//                 break ;
//             }
//         }
//     }
// }

void EPollManager::acceptConnection(int serverFd) {
    std::cout << "accept co: " << serverFd << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        perror("Error: avec connect() function...");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER: Nouvelle connexion acceptée : FD " << newClientFd << std::endl;
    
    // Mapping client fd to the correct server and socket
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        for (size_t j = 0; j < this->_servers[i].getListeningSocket().size(); ++j) {
            if (serverFd == this->_servers[i].getListeningSocket()[j].getSockFd()) {
                this->_servers[i].getListeningSocket()[j].addClientToListeningSocket(newClientFd);

                std::cout << "client fd nb " << newClientFd << " ajouté au server " << this->_servers[i].getListeningSocket()[j].getSockFd() << std::endl;
                addSocketToEpoll(newClientFd);
                break;
            }
        }
    }
}

void EPollManager::handleClientRequest(int clientFd, Server& serv) {
    std::string buf;
    std::string request;
    ssize_t bytes_read;

    buf.resize(BUFFER_SIZE);
    std::cout << "+++++++++++++++++++++++++++++++++++++ Debut handleClientRequest" << std::endl;

    while (request.find("\r\n\r\n") == std::string::npos) {
        bytes_read = recv(clientFd, &buf[0], BUFFER_SIZE, 0);
        
        std::cout << "clientFd: " << clientFd << std::endl;
        if (bytes_read <= 0) {

            if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
                perror("epoll_ctl: remove clientFd");
            }
            std::cerr << "Erreur handleClientRequest" << std::endl;
            // FD_CLR(clientFd, this->_epollFd);
            close(clientFd);
            break;
        }
        request += buf;
    }

    ErrorManagement err;
    RequestParser req_parser(err);

    req_parser.parseRequest(request, clientFd);

    if (err.getErrorCode() != 0) {
        return;
    }

    ProcessRequest process_req(&serv, req_parser, err);
    ResponseMaker resp(err, process_req);
    std::string response = resp.getFinalResponse();
    size_t size = response.size();

    std::cout << "Reponse: " << std::endl;
    std::cout << response << std::endl;
    send(clientFd, &response[0], size, 0);

    if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
        perror("epoll_ctl: remove clientFd");
    }

    close(clientFd);
    std::cout << "+++++++++++++++++++++++++++++++++++++ Fin handleClientRequest" << std::endl;
}



// void EPollManager::acceptConnection(int serverFd) {
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);

//     while (true) {
//         int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
//         if (newClientFd == -1) {
//             if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                 // plus de connexions à accepter
//                 break;
//             } else {
//                 perror("accept");
//                 break;
//             }
//         }

//         std::cout << "Nouvelle connexion acceptée : FD " << newClientFd << std::endl;

//         // rendre client non-bloquant aussi (optionnel mais conseillé)
//         int flags = fcntl(newClientFd, F_GETFL, 0);
//         fcntl(newClientFd, F_SETFL, flags | O_NONBLOCK);

//         // ajout à ton epoll + liste clients
//         addSocketToEpoll(newClientFd);
//         // ajoute à la bonne ListeningSocket ici !
//     }
// }



/*=====REECRITURE FONCTIONS PAR CHARLES=====*/
// void	EPollManager::handleClientRequest(int clientFd, Server& serv)
// {
//     for (size_t i = 0; i < serv.getListeningSocket().size(); ++i) {
//         std::cout << serv.getListeningSocket()[i].getPort() << std::endl;
//     }
    
//     std::string		buf;
// 	std::string		request;
// 	ssize_t			bytes_read;

// 	buf.resize(BUFFER_SIZE);
// 	std::cout << std::endl << std::endl << std::endl;
// 	std::cout << "+++++++++++++++++++++++++++++++++++++ Debut handleClientRequest" << std::endl;
// 	while (request.find("\r\n\r\n") == std::string::npos)
// 	{
// 		// buf.resize(BUFFER_SIZE);
//         std::cout << "clientFd :" << clientFd << std::endl;
//         std::cout << "&buf[0]: " << &buf[0] << std::endl;
// 		bytes_read = recv(clientFd, &buf[0], BUFFER_SIZE, 0);
        
// 		std::cout << buf << std::endl;
//         if (bytes_read <= 0)
// 		{
// 			std::cerr << "erreur handleClientRequest" << std::endl;
// 			close(clientFd);
//             // _fdToListeningSocket.erase(clientFd);
//             // _fdToServer.erase(clientFd);
// 			break;
// 		}
//         // buf.resize(bytes_read);  // Ne garde que ce qui a été lu
// 		request += buf;
// 	}

// 	ErrorManagement	err;
// 	RequestParser	req_parser(err);

// 	req_parser.parseRequest(request, clientFd);
// 	// std::cout << "Elem de la requete apres le parse :" << std::endl;
// 	// std::cout << "Method : " << req_parser.getMethod() << std::endl;
// 	// std::cout << "URI : " << req_parser.getURI() << std::endl;
// 	// std::cout << "HTTP version : " << req_parser.getHTTP() << std::endl;
// 	// std::cout << std::endl;
// 	if (err.getErrorCode() != 0)
// 	{
// 		return ;
// 	}

// 	ProcessRequest	process_req(&serv, req_parser, err);
// 	if (req_parser.getIsCgi() == true)
// 	{
// 		// GREG TU TE DEMERDES POUR LA REP
// 	}

// 	ResponseMaker	resp(err, process_req);
// 	std::string		response = resp.getFinalResponse();
// 	size_t				size = response.size();

// 	std::cout << "Reponse :" << std::endl;
// 	std::cout << response << std::endl;
// 	send(clientFd, &response[0], size, 0);
//     std::cout << "+++++++++++++++++++++++++++++++++++++ Fin handleClientRequest" << std::endl;

// }
