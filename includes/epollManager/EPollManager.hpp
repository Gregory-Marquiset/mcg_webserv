#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../WebServ.hpp"

// strace pour check les process

// -> en gros c est une structure de controle 
// to poll = interroger/sonder -> donc pollfd = interrogation/controle des fd (des sockets)
// struct pollfd {
//     int   fd;         /* Descripteur de fichier */
//     short events;     /* Événements attendus    */
//     short revents;    /* Événements détectés    */
// };

// le PollManager je l init comment ?

// comment je fais le lien entre les servers et le pollfd ? 

// Server va avoir la ListeningSocket + readyBlock

class EPollManager {

  private:
    std::vector<Server*> _servers;
    int  _newSocketFd;
    std::vector<pollfd> _allSockets; // du coup je recup sockets clients et server 

  public:
    EPollManager();
    ~EPollManager();

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