#include "../../includes/server/ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklg) : ASocket(domain, service, protocol) {

  this->_port = port;
  this->_interface = interface;
  this->_backlog = bklg;

  // define address structure
  this->getAddress().sin_family = domain;
  this->getAddress().sin_port = htons(port);
  this->getAddress().sin_addr.s_addr = htonl(interface);

  // bind the socket to a port (sans ca on ne peut pas listen)
  this->_connection = bindToNetwork(getSockFd(), getAddress());
  testConnection(_connection);
  
  // listen
  startListening();
  testConnection(this->_listening);
}

ListeningSocket::~ListeningSocket() {};

int ListeningSocket::bindToNetwork(int sockFd, struct sockaddr_in address) {

  int yes = 1;
  if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, (void*)&yes, sizeof(yes)) < 0) {
      throw (std::runtime_error("Error: setsockopt() failed..."));
  }
  return bind(sockFd, (struct sockaddr *)&address, sizeof(address));
}

void ListeningSocket::startListening() {
    this->_listening = listen(getSockFd(), this->_backlog);
}

int ListeningSocket::getPort() const {
  return (this->_port);
}

void ListeningSocket::addClientToListeningSocket(int newClientFd) {
  this->_clientsFd.push_back(newClientFd);
}

std::vector<int>& ListeningSocket::getClientsFd() {
  return (this->_clientsFd);
}

/* ================= HELPER TO PRINT STUFF ======================== */

void ListeningSocket::printListeningSocketArgs(int domain, int service, int protocol, int port, u_long interface, int backlog) {
  std::cout << "domain = " << domain << " (c est le domaine d adresse utilise par le socket, soit IPV4 -> AF_INET)" << std::endl;
  std::cout << "service = " << service << " (ca specifie le type de connection qu on veut etablir, soit SOCK_STREAM -> TCP)" << std::endl;
  std::cout << "protocol = " << protocol << " (c est en rapport avec TCP, ici ce sera par defaut 0)" << std::endl;
  std::cout << "port = " << port << " (le port sur lequel le server ecoutera les connections entrantes, cad sur lequel le server se connectera)" << std::endl;
  std::cout << "interface = " << interface << " (c est l IP de l interface reseau a utiliser par le socket, ici on va choisir le flag INADDR_ANY pour que le socket puisse ecouter sur toutes les interfaces reseau dispo de la machine)" << std::endl;
  std::cout << "backlog = " << backlog << " (nombre de clients autorises a se connect au serve)" << std::endl;
}