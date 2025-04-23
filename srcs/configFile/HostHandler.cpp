#include "../../includes/configFile/HostHandler.hpp" 

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

HostHandler::HostHandler() {};

HostHandler::~HostHandler() {};

/* ================= SETTER - GETTER ======================== */

void HostHandler::setHostName(std::vector<std::string> hostName) {
    this->_hostName = hostName;
}    

void HostHandler::setHostFormat(int hostFormat) {
    this->_hostFormat = hostFormat;
}

std::vector<std::string>& HostHandler::getHostName() {
    return (this->_hostName);
}

int HostHandler::getHostFormat() const {
    return (this->_hostFormat);
}

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

// si 3 dots et entre 4 et 12 digits alors je presume qu on me donne server_name sous forme d IP -> c est completement arbitraire

int isLoopbackRange(const std::string& ip_str) {
    struct in_addr addr; // c est pour stocker l addr sous format binaire
    if (inet_aton(ip_str.c_str(), &addr) == 0) { // ca c est ce qui fait la conversion
        throw (std::runtime_error("Error: Invalid IP in .conf -> This is an out of range address"));
        return (0);
    }

    u_long ip_numeric = ntohl(addr.s_addr); //  addr.s_addr contient l IP en big endian (ordre reseau) puis nhohl converti en ordre hote pour faire le op de bits
    if ((ip_numeric & 0xFF000000) == 0x7F000000) { // pour check si les 8 bits les plus a gauches sont 01111111 (0x7F) soit 1270.0.0.0/8 
        return (1);
    }
    return 0;
}

int isBroadcastHardcoded(const std::string& ip) {
    if (ip == "127.0.0.0" || ip == "127.255.255.255")
        return (1);
    return (0);
}

void HostHandler::filter(std::string hostLine) {
    
    int countDot = 0;
    int countDigit = 0;
    
    for (size_t  i = 0; i < hostLine.size(); ++i) {
        if (hostLine[i] == '.') {
            countDot++;
        }
        if (std::isdigit(hostLine[i])) {
            countDigit++;
        }
    }
   
    if (countDot == 3 && countDigit >= 4 && countDigit <= 12) {
    
        if (isBroadcastHardcoded(hostLine) == 1) {
            throw (std::runtime_error("Error: Invalid IP in .conf -> This is a broadcast address"));
        } else if (isLoopbackRange(hostLine) == 1) {
            this->setHostFormat(1);
            this->_hostName.push_back(hostLine);
        } else {
            throw (std::runtime_error("Error: Invalid IP in .conf -> This is not a loopback address"));
        }
    }
    else {
        throw (std::runtime_error("Error: Invalid IP in .conf -> This is an out of range address"));
    }
}

// check pour invalid IP

void HostHandler::parseIp(std::string hostLine) {

    std::string::size_type dotPos = -1;
    
    int start = 0;

    while ((dotPos = hostLine.find('.', dotPos + 1)) != std::string::npos) {
        
        std::string part = hostLine.substr(start, dotPos - start);

        for (size_t i = 0; i < part.size(); ++i) {
            if (!std::isdigit(part[i])) {
                throw (std::invalid_argument("Error: Invalid IP in .conf -> one of the character is not a digit"));
            }
        }
        if (dotPos != std::string::npos) {
                            
            if (myStoi(part) < 0 || myStoi(part) > 255) {
                throw (std::invalid_argument("Error: Invalid IP in .conf -> out of range"));
            }
        }
        start = (dotPos + 1);
    }

    std::string lastPart = hostLine.substr(start);

    for (size_t i = 0; i < lastPart.size(); ++i) {
        if (!std::isdigit(lastPart[i])) {
            throw (std::invalid_argument("Error: Invalid IP in .conf -> one of the character is not a digit"));
        }
    }
    if (myStoi(lastPart) < 0 || myStoi(lastPart) > 255) {
        throw (std::invalid_argument("Error: Invalid IP in .conf -> out of range"));
    }
}

void HostHandler::checkListenFormat(std::string listenLine, ServerBlock& server) {
    
    int onlyPort = 1;

    for (size_t  i = 0; i < listenLine.size(); ++i) { // je cherche les ":"
        if (!std::isdigit(listenLine[i]))
            onlyPort = 0;
    }

    if (onlyPort == 1) { // cas ou il n y a pas d ip possible sur la ligne listen

        int tmpRes = myStoi(listenLine);
        
        if (tmpRes < 0 || tmpRes > 65535) {
            throw (std::invalid_argument("Error: Invalid Port"));
        }
        else {
            server.addPort(tmpRes);
        }
    }
    else { // cas ou il y a une ip possible sur la ligne listen

        size_t pos = listenLine.find(':');

        std::string tmpPort = listenLine.substr(pos + 1);
        for (size_t i = 0; i < tmpPort.size(); ++i) {
            if (!std::isdigit(tmpPort[i])) {
                throw (std::invalid_argument("Error: Invalid Port"));
            }
        }

        int tmpRes = myStoi(tmpPort);
        if (tmpRes < 0 || tmpRes > 65535) {
            throw (std::invalid_argument("Error: Invalid Port"));
        }
        else {
            server.addPort(tmpRes);
        }
        if (pos != std::string::npos) {
            std::string tmpIp = listenLine.substr(0, pos);
            parseIp(tmpIp);
            filter(tmpIp);
        }
    }
}