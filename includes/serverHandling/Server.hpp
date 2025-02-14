#ifndef SERVER_HPP
#define SERVER_HPP

#include "../WebServ.hpp"
#include "../socketInit/ListeningSocket.hpp"

// -> en gros c est une structure de controle 
// to poll = interroger/sonder -> donc pollfd = interrogation/controle des fd (des sockets)
// struct pollfd {
//     int   fd;         /* Descripteur de fichier */
//     short events;     /* Événements attendus    */
//     short revents;    /* Événements détectés    */
// };

class Server {

  private:
    ListeningSocket* _listeningSocket;
    int  _newSocketFd;
    char _buffer[1024];
    std::vector<pollfd> _allSockets;

  public:
    Server(int domain, int service, int protocol, int port, u_long interface, int bcklg);
    ~Server();

    /* getters */

    ListeningSocket *getListeningSocket();
    char* getBuffer();
    std::vector<pollfd> &getAllSockets();

    /* gestion des evenements/sockets clientes par le server */

    int acceptConnection();
    void addClientToPollFdStruct(int fd);
    void serverHandlingEvents(std::string root, std::string index);
    

    
    void handleClientRequest(int clientFd, std::string root, std::string index);

    void printMessageFromClient(int clientFd, int bytesRead);
    void responseFromServer(int clientFd, const std::string& request, std::string root, std::string index);
};

#endif