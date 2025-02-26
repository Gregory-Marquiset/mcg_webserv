#include "../../includes/socketInit/ASocket.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

// create/initialise le socketFd (grace a la fonction socket())

ASocket::ASocket(int domain, int service, int protocol) {
  this->_sockFd = socket(domain, service, protocol);
  testConnection(this->_sockFd);
}

ASocket::~ASocket() {};

/* ================= SETTERS - GETTERS ======================== */

void ASocket::setConnection(int con) {
  this->_connection = con;
}

struct sockaddr_in& ASocket::getAddress() {
  return (this->_address);
}

int ASocket::getSockFd() {
  return (this->_sockFd);
}

int ASocket::getConnection() {
  return (this->_connection);
}

/* function qui check for success or not */

void ASocket::testConnection(int item) {
    if (item < 0) {
        perror("Failed to connect...");
        exit(EXIT_FAILURE);
    }
}

/* ================= HELPER TO PRINT STUFF ======================== */

// void ASocket::printSocketInfo() {
//   std::cout << "Domain = " << this->address.sin_family << " (IPV4)" << std::endl;
//   std::cout << "Port = " << this->address.sin_port << std::endl;
//   std::cout << "Interface = " << this->address.sin_addr.s_addr << std::endl;
// }