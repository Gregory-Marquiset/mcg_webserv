#include "../../includes/configFile/HostHandler.hpp" 

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

HostHandler::HostHandler() {};

HostHandler::~HostHandler() {};

/* ================= SETTER - GETTER ======================== */

void HostHandler::setHostName(std::string hostName) {
    this->_hostName = hostName;
}    

void HostHandler::setHostFormat(int hostFormat) {
    this->_hostFormat = hostFormat;
}

std::string HostHandler::getHostName() const {
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
        this->setHostFormat(1);
        this->setHostName(hostLine);
    }
    else
        this->setHostFormat(0);
}

// check pour invalid IP: 0.0.0.0 à 255.255.255.255

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
 
    HostHandler host;

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