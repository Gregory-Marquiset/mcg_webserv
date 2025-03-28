#include "../../includes/epollManager/EPollManager.hpp"
#include "../../includes/request/RequestParser.hpp"
#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

// epoll_create1(0) crée un file descriptor epoll.
// S'il échoue, un message d'erreur est affiché, et le programme quitte immédiatement.

/*
EPollManager::EPollManager(const std::vector<Server>& servers) {

    this->_servers = servers;

    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // parcourt tous les serveurs et ajoute leur socket d'écoute (socket principale)
    // au file descriptor epoll via addSocketToEpoll.
    std::vector<Server>::iterator it;
    for (it = this->_servers.begin(); it != this->_servers.end(); ++it) {
        addSocketToEpoll(it->getListeningSocket().getSockFd());
    }
}
    */


// le monitor sait quels sont les servers par defauts

EPollManager::EPollManager(std::vector<Server>& servers) : _servers(servers) {
    
    this->_epollFd = epoll_create1(0); // c est le fd monitor
    if (this->_epollFd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    determineDefaultServersAccordingToPort(this->_servers);
}
/*
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

                for (size_t k = 0; k < portAlreadyAssigned.size(); ++k) {
                    if (portToCheck == portAlreadyAssigned[k]) {
                        res[portToCheck] = 0;
                        servers[i].addStatus(res);
                        break ;
                    } 
                }
                res[portToCheck] = 1;
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
*/


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

EPollManager::~EPollManager() {

    close(_epollFd);
}

/* ================= UTILS ======================== */

// Cette fonction ajoute une socket spécifique au système epoll
// pour surveiller les événements de lecture (EPOLLIN).
// Cet événement est configuré pour détecter les données disponibles en lecture (EPOLLIN).
// epoll_ctl() ajoute (EPOLL_CTL_ADD) la socket fd au descripteur epoll _epollFd
void EPollManager::addSocketToEpoll(int fd) {

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl: ajout socket");
        exit(EXIT_FAILURE);
    }
}

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
                if (fd == _servers[j].getListeningSocket().front().getSockFd()) { // changer le .front() faire en sorte d avoir la bonne listening socket
                    acceptConnection(fd);
                    isServerSocket = 1;
                    break;
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
        if (serverFd == _servers[j].getListeningSocket().front().getSockFd()) {
            clientToServerMap[newClientFd] = &_servers[j];
            break;
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
			//close(clientFd);
			std::cerr << "erreur handleClientRequest" << std::endl;
			close(clientFd);
			break;
		}
		request += buf;
	}

	RequestParser	req_parser;

	req_parser.parseRequest(request, clientFd);
	std::cout << "Elem de la requete apres le parse :" << std::endl;
	std::cout << "Method : " << req_parser.getMethod() << std::endl;
	std::cout << "URI : " << req_parser.getURI() << std::endl;
	std::cout << "HTTP version : " << req_parser.getHTTP() << std::endl;
	std::cout << std::endl;
	if (req_parser.getErrorCode() != 0)
	{
		return ;
	}

	ProcessRequest	process_req(serv, req_parser);
	if (req_parser.getIsCgi() == true)
	{
		//GREG TU TE DEMERDES POUR LA REP
	}


	//Pour tester:
	std::string		_response;
	std::ifstream	resp_file(process_req.getFinalPath().c_str());

	if (resp_file.is_open())
	{
		std::stringstream	content;
		std::stringstream	size;
		std::string			body;
		std::string			body_size;

		content << resp_file.rdbuf();
		body = content.str();
		size << body.size();
		body_size = size.str();

		_response = "HTTP/1.1 200 OK\r\n";
		_response += "Content-Type: text/html\r\n";
		_response += "Content-Length: " + body_size + "\r\n";
		_response += "\r\n";
		_response += content.str();

		std::cout << "Reponse :" << std::endl;
		std::cout << _response << std::endl;
	}
	send(clientFd, &_response[0], _response.size(), 0);
}
