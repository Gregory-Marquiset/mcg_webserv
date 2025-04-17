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

int myStoi2(std::string& s) {
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
    
    if (countDot == 3 && countDigit >= 4 && countDigit <= 12)
        this->setHostFormat(1);
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
                std::cout << "Error: Invalid IP in .conf\n";    
                exit(EXIT_FAILURE);
            }
        }
        if (dotPos != std::string::npos) {
                            
            if (myStoi2(part) < 0 || myStoi2(part) > 255) {
                std::cout << "Error: Invalid IP in .conf\n";    
                exit(EXIT_FAILURE);
            }
        }
        start = (dotPos + 1);
    }

    std::string lastPart = hostLine.substr(start);

    for (size_t i = 0; i < lastPart.size(); ++i) {
        if (!std::isdigit(lastPart[i])) {
            std::cout << "Error: Invalid IP in .conf\n";    
            exit(EXIT_FAILURE);
        }
    }
    if (myStoi2(lastPart) < 0 || myStoi2(lastPart) > 255) {
        std::cout << "Error: Invalid IP in .conf\n";    
        exit(EXIT_FAILURE);
    }
    
    this->setHostName(hostLine);
}

void HostHandler::checkListenFormat(std::string listenLine, ServerBlock& server) {
 
    HostHandler host;

    // std::cout << "check listen: " << listenLine << std::endl;
    int onlyPort = 1;

    for (size_t  i = 0; i < listenLine.size(); ++i) {
        if (!std::isdigit(listenLine[i]))
            onlyPort = 0;
    }
    // std::cout << "onlyPort = " << onlyPort << std::endl;
 
    if (onlyPort == 1) {
        int tmpRes = myStoi2(listenLine);
        // std::cout << "ppppp" << tmpRes << std::endl;
        if (tmpRes < 0 || tmpRes > 65535) {
            throw (std::invalid_argument("Error: Invalid Port"));
        }
        else {
            // std::cout << "Valid Port" << std::endl;
            server.addPort(tmpRes);
            // std::cout << "port " << tmpRes << " added\n";
            // server.setIp("127.0.0.1");
        }
    }
    else {
        size_t pos = listenLine.find(':');
        std::string tmpPort = listenLine.substr(pos + 1);
        for (size_t i = 0; i < tmpPort.size(); ++i) {
            if (!std::isdigit(tmpPort[i])) {
                throw (std::invalid_argument("Error: Invalid Port"));
            }
        }
        std::cout << "tmpPort = " << tmpPort << std::endl;
        int tmpRes = myStoi2(tmpPort);
        // std::cout << "ppppp" << tmpRes << std::endl;
        if (tmpRes < 0) {
            throw (std::invalid_argument("Error: Invalid Port"));
        }
        else {
            // std::cout << "Valid Port" << std::endl;
            server.addPort(tmpRes);
        }
        if (pos != std::string::npos) {
 
            std::string tmpIp = listenLine.substr(0, pos);
            // std::cout << "res IP = " << tmpIp << std::endl;
            std::string::size_type dotPos = -1;

            int start = 0;
            while ((dotPos = listenLine.find('.', dotPos + 1)) != std::string::npos) {
                std::string part = tmpIp.substr(start, dotPos - start);
                for (size_t i = 0; i < part.size(); ++i) {
                    if (!std::isdigit(part[i])) {

                        std::cout << "Error: Invalid IP in .conf\n";
                        exit(EXIT_FAILURE);
                    }
                }
                if (dotPos != std::string::npos) {
 
                    // std::cout << "part " << myStoi2(part) << std::endl;
 
                    if (myStoi2(part) < 0 || myStoi2(part) > 255) {
                        std::cout << "Error: Invalid IP in .conf\n";
                        exit(EXIT_FAILURE);
                    }
                }
                start = (dotPos + 1);
            }
            std::string lastPart = tmpIp.substr(start);

            for (size_t i = 0; i < lastPart.size(); ++i) {

                if (!std::isdigit(lastPart[i])) {

                    std::cout << "Error: Invalid IP in .conf\n";

                    exit(EXIT_FAILURE);
                }
            }
            // std::cout << "part " << myStoi2(lastPart) << std::endl;
            if (myStoi2(lastPart) < 0 || myStoi2(lastPart) > 255) {
                std::cout << "Error: Invalid IP in .conf\n";
                exit(EXIT_FAILURE);
            }
            // server.addHost(host);
        }
    }
}