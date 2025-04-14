#include "../../includes/epollManager/EPollManager.hpp"
#include "../../includes/request/RequestParser.hpp"
#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

// epoll_create1(0) crée un file descriptor epoll.
// S'il échoue, un message d'erreur est affiché, et le programme quitte immédiatement.

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
        if (serverFd == _servers[j].getListeningSocket().getSockFd()) {
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

	ResponseMaker	resp(err, process_req);
	std::string		response = resp.getFinalResponse();
	size_t				size = response.size();

	std::cout << "Reponse :" << std::endl;
	std::cout << response << std::endl;
	send(clientFd, &response[0], size, 0);
	//if (req_parser.getConnection() != "keep-alive")
	close(clientFd);
	//std::cout << std::endl << "ca crash pas ici" << std::endl;
}
