#include "../../includes/epollManager/EPollManager.hpp"
#include "../../includes/request/RequestParser.hpp"
#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

// epoll_create1(0) crée un file descriptor epoll.
// S'il échoue, un message d'erreur est affiché, et le programme quitte immédiatement.

EPollManager::EPollManager(std::vector<Server>& servers) : _servers(servers) {

    this->_epollFd = epoll_create1(0); // peut etre rajouter un flag lorsqu on aura la gestion des cgi
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
                        this->_getServerBasedOnServerFd[fd] = &servers[i];
                        this->_getListeningSocketBasedOnServerFd[fd] = &servers[i].getListeningSocket()[k];
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

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw (std::runtime_error("Error: accept function failed..."));
    }
}

void EPollManager::acceptConnection(int serverFd) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int newClientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newClientFd < 0) {
        throw std::runtime_error("Error: accept function failed...");
        return ;
    }

    // mettre en non-bloquant les sockets clientes:
    int flags = fcntl(newClientFd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("Error getting socket flags");
        return ;
    }

    if (fcntl(newClientFd, F_SETFL, flags | O_NONBLOCK) == -1) {
        close(newClientFd);
        throw std::runtime_error("Error setting socket to non-blocking");
        return ;
    }

    ListeningSocket* listeningSocket = this->_getListeningSocketBasedOnServerFd[serverFd];
    Server* server = this->_getServerBasedOnServerFd[serverFd];

    if (!listeningSocket || !server) {
        close(newClientFd);
        return ;
    }

    listeningSocket->addClientToListeningSocket(newClientFd);
    this->_getServerBasedOnClientFd[newClientFd] = server; // en regardant  l int newClientFd, je l associe a server et je creer donc une entree dans ma map

    addSocketToEpoll(newClientFd);

    // std::cout << "Client fd " << newClientFd << " ajouté au serveur sur fd " << serverFd << std::endl;
}

extern volatile sig_atomic_t exitFlag;

void EPollManager::run() {

    const int MAX_EVENTS = 10000;
    this->_events.clear();
    this->_events.resize(MAX_EVENTS);

    while (exitFlag == 0) {

        if (exitFlag == 1) {
            break;
        }

        int n = epoll_wait(this->_epollFd, this->_events.data(), MAX_EVENTS, -1); // n c est le fd sur lequel epollin a lu quelque chose
        if (n == -1) {
            if (exitFlag == 1) {
                break;
            }
            throw std::runtime_error("Error: epoll_wait() failed...");
        }

        for (int i = 0; i < n; ++i) {
            int fd = this->_events[i].data.fd; // le fd sur lequel event repere (c est le fd actif)

            if (std::find(this->_validListeningSockets.begin(), this->_validListeningSockets.end(), fd) != this->_validListeningSockets.end()) { // si server
                acceptConnection(fd);
            }
            // si c'est un client
            else {
                std::map<int, Server*>::iterator it = this->_getServerBasedOnClientFd.find(fd);
                if (it != this->_getServerBasedOnClientFd.end()) {
                    Server* realServer = it->second;
                    handleClientRequest(fd, realServer);
                }
                else {
                    std::cerr << "Erreur: fd client " << fd << " non trouvé dans aucune socket serveur. Fermeture." << std::endl;
                    epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
        }
    }
}

void EPollManager::handleClientRequest(int clientFd, Server* serv)
{
	std::string buf;
	std::string request;
	ssize_t bytes_read;

	while (request.find("\r\n\r\n") == std::string::npos)
	{
		buf.resize(BUFFER_SIZE);
		bytes_read = recv(clientFd, &buf[0], BUFFER_SIZE, 0);
		if (bytes_read == 0)
		{
			std::cout << "Client " << clientFd << " a fermé la connexion." << std::endl;
			epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
			close(clientFd);
			return;
		}
		else if (bytes_read < 0)
		{
			std::cerr << "Erreur lors de la lecture sur le clientFd: " << clientFd << std::endl;
			epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
			close(clientFd);
			return;
		}
		request.append(buf, 0, bytes_read);
	}

	// Parser la requête
	ErrorManagement err;
	RequestParser req_parser(err);
	req_parser.parseRequest(request, clientFd);


	// Traiter la requête
	ProcessRequest process_req(serv, req_parser, err);

	if (err.getErrorCode() == 0)
		process_req.processRequest();

	ResponseMaker resp(err, process_req);
	std::string response = resp.getFinalResponse();

	size_t response_size = response.size();
	ssize_t bytes_sent = send(clientFd, &response[0], response_size, 0);
	if (bytes_sent <= 0)
		perror("Error while sending http response");

	// Retirer le client du epoll et fermer
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1)
		perror("epoll_ctl: remove clientFd");
	close(clientFd);
}

void EPollManager::clean() {
    for (size_t i = 0; i < this->_servers.size(); ++i) {
        std::vector<ListeningSocket>& sockets = this->_servers[i].getListeningSocket();
        for (size_t j = 0; j < sockets.size(); ++j) {
            std::vector<int>& clients = sockets[j].getClientsFd();
            for (size_t k = 0; k < clients.size(); ++k) {
                epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clients[k], NULL);
                close(clients[k]);
            }
            int sockFd = sockets[j].getSockFd();
            if (sockFd != -1) {
                epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, sockFd, NULL);
                close(sockFd);
            }
        }
    }
    close(this->_epollFd);
}
