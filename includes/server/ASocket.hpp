#ifndef ASOCKET_HPP
#define ASOCKET_HPP

#include "../WebServ.hpp"

class ASocket {

  private:
    struct sockaddr_in _address;
    int _sockFd;
    int _connection;

  public:
    ASocket(int domain, int serice, int protocol);
    virtual ~ASocket();

    /* getters */
    
    struct sockaddr_in& getAddress();
    int getSockFd() const;
    int getConnection();
    
    /* setter */

    void setConnection(int con);
    
    /* function qui check for success or not */

    void testConnection(int);

    /* helper */

    void printSocketInfos(int domain, int service, int protocol);
};

#endif