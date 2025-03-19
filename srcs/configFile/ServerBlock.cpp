#include "../../includes/configFile/ServerBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ServerBlock::ServerBlock() {};

ServerBlock::~ServerBlock() {};

/* ================= SETTERS ======================== */

void ServerBlock::setServer(std::string server) {
    this->_server = server;
}

void ServerBlock::setPort(const int& port) {
    this->_port.push_back(port);
    std::cout << "set port function " << port << std::endl;
}

void ServerBlock::setServerName(std::string serverName) {
    this->_serverName = serverName;
}

void ServerBlock::setLocation(std::vector<LocationBlock> location) {
    this->_location = location;
}

void ServerBlock::setRoot(std::string root) {
    this->_root = root;
}

void ServerBlock::setIndex(std::string index) {
    this->_index = index;
}

void ServerBlock::setAllowMethods(std::string allowMethods) {
    this->_allowMethods = allowMethods;
}

void ServerBlock::setCgiExtension(std::string cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

void ServerBlock::setClientMaxBodySize(std::string client_max_body_size) {
    this->_client_max_body_size = client_max_body_size;
}

/* ================= GETTERS ======================== */

std::string ServerBlock::getServer() const {
    return (this->_server);
}

std::vector<int> ServerBlock::getPort() const {
    return (this->_port);
}

std::string ServerBlock::getServerName() const {
    return (this->_serverName);
}

std::vector<LocationBlock> ServerBlock::getLocation() const {
    return (this->_location);
}

std::string ServerBlock::getRoot() const {
    return (this->_root);
}

std::string ServerBlock::getIndex() const {
    return (this->_index);
}

std::string ServerBlock::getAllowMethods() const {
    return (this->_allowMethods);
}

std::string ServerBlock::getCgiExtension() const {
    return (this->_cgiExtension);
}

std::string ServerBlock::getClientMaxBodySize() const {
    return (this->_client_max_body_size);
}

/* ================= HELPERS ======================== */

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

void ServerBlock::addLocationBlock(const LocationBlock& location) {
    this->_location.push_back(location);
}

/* IP decoupe */

void ServerBlock::setIp(std::string ip) {
    this->_ip = ip;
}

std::string ServerBlock::getIp() const {
    return (this->_ip);
}

// check pour invalid IP: 0.0.0.0 à 255.255.255.255
// check pour invalid port: Les ports vont de 0 à 65535, mais tous ne sont pas recommandés

void ServerBlock::checkListenFormat(std::string listenLine, ServerBlock& server) {

    std::cout << "check listen: " << listenLine << std::endl;
    int onlyPort = 1;

    for (size_t  i = 0; i < listenLine.size(); ++i) {
        if (!std::isdigit(listenLine[i]))
            onlyPort = 0;
    }

    std::cout << "onlyPort = " << onlyPort << std::endl;

    if (onlyPort == 1) {
        int tmpRes = myStoi(listenLine);
        // std::cout << "ppppp" << tmpRes << std::endl;
        if (tmpRes < 0) {
            std::cout << "Error: Invalid Port" << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            // std::cout << "Valid Port" << std::endl;
            server.setPort(tmpRes);
            std::cout << "port " << tmpRes << " added\n";
            server.setIp("127.0.0.1");

        }
    }
    else {
        
        size_t pos = listenLine.find(':');
        
        std::string tmpPort = listenLine.substr(pos + 1);
        for (size_t i = 0; i < tmpPort.size(); ++i) {
            if (!std::isdigit(tmpPort[i])) {
                std::cout << "Error: Invalid Port in .conf\n";
                exit(EXIT_FAILURE);
            }
        }
        std::cout << "tmpPort = " << tmpPort << std::endl;
        int tmpRes = myStoi(tmpPort);
        // std::cout << "ppppp" << tmpRes << std::endl;
        if (tmpRes < 0) {
            std::cout << "Error: Invalid Port in .conf" << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            // std::cout << "Valid Port" << std::endl;
            server.setPort(tmpRes);
        }
        if (pos != std::string::npos) {
            
            std::string tmpIp = listenLine.substr(0, pos);
            
            std::cout << "res IP = " << tmpIp << std::endl;
            
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
                    
                    std::cout << "part " << myStoi(part) << std::endl;
                    
                    if (myStoi(part) < 0 || myStoi(part) > 255) {
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
            std::cout << "part " << myStoi(lastPart) << std::endl;
            if (myStoi(lastPart) < 0 || myStoi(lastPart) > 255) {
                std::cout << "Error: Invalid IP in .conf\n";
                exit(EXIT_FAILURE);
            }
            
            server.setIp(tmpIp);
        }
    }
}



// if ()
// check si only digit
// si oui ca veut dire que j ai juste le port
// faire un check pour > 0 && < 65535
// si non return error
// si oui set port 

// si char alors ip presente
// substr pour les : -> res = ip
// check ip valide 0.0.0.0 à 255.255.255.255
// si non return error
// si oui set ip

// donc en gros inclure les setters dans les verif sinon return error de parse 

/* ================= SET OBJECT SERVERBLOCK ======================== */




std::vector<ServerBlock> ServerBlock::getAllServerBlocks(RecupBlockContent rawConfig) {
    std::vector<ServerBlock> cleanServers;
    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    for (std::vector<Block>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
        
        // int flag = 0;
        
        if (it->getName() == "server") {
            
            ServerBlock oneServerBlock;
            
            // std::multimap<std::string, std::string> directive = it->getDirective();

            // if (directive.find("listen") != directive.end()) {
            //     std::cout << &oneServerBlock << std::endl;
            //     checkListenFormat(directive.find("listen")->second, oneServerBlock);
            //     std::cout << oneServerBlock.getPort().size() << std::endl;
            // }


            std::multimap<std::string, std::string> directive = it->getDirective();

            // Récupérer toutes les occurrences de "listen"
            std::pair<std::multimap<std::string, std::string>::iterator,
                    std::multimap<std::string, std::string>::iterator> range;

            range = directive.equal_range("listen");

            // Parcourir et traiter chaque port
            for (std::multimap<std::string, std::string>::iterator it = range.first; it != range.second; ++it) {
                checkListenFormat(it->second, oneServerBlock);
            }

            // if (directive.find("server_name") != directive.end()) {
            //     oneServerBlock.setServerName(directive["server_name"]);
            // }
            // if (directive.find("root") != directive.end()) {
            //     // checkRootFormat();
            //     oneServerBlock.setRoot(directive["root"]);
            // }
            // if (directive.find("index") != directive.end()) {
            //     // checkIndexFormat();
            //     oneServerBlock.setIndex(directive["index"]);
            // }

            // const std::vector<Block>& locations = it->getChildBlock();
            
            // for (std::vector<Block>::const_iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {
                
            //     LocationBlock locBlock;
                
                
            //     std::map<std::string, std::string> locationDirective = itLocation->getDirective();
                
            //     if (locationDirective.find("root") != locationDirective.end()) {
            //         // checkRootFormat();
            //         locBlock.setRoot(locationDirective["root"]);
            //     }
            //     if (locationDirective.find("index") != locationDirective.end()) {
            //         // checkIndexFormat();
            //         locBlock.setIndex(locationDirective["index"]);
            //     }
            //     if (locationDirective.find("allow_methods") != locationDirective.end()) {
            //         // checkAllowMethodsFormat();
            //         locBlock.setAllowMethods(locationDirective["allow_methods"]);
            //         flag = 1;
            //     }
            //     if (locationDirective.find("cgi_extension") != locationDirective.end()) {
            //         // check CgiExtensionFormat();
            //         locBlock.setCgiExtension(locationDirective["cgi_extension"]);
            //     }
            //     if (locationDirective.find("client_max_body_size") != locationDirective.end()) {
            //         locBlock.setClientMaxBodySize(locationDirective["client_max_body_size"]);
            //     }
                
            //     locBlock.setPath(itLocation->getName());
            //     // checkForPathFormat();
            //     oneServerBlock.addLocationBlock(locBlock); // en faite ca c est comme le setter
            // }

            // if (flag == 0) {
            //     std::cout << "coucou\n";
            //     oneServerBlock.setAllowMethods("GET POST DELETE");
            // }

            cleanServers.push_back(oneServerBlock);
        }
    }
    return (cleanServers);
}

// OKISH check que plusieurs serveurs peuvent etre conf sur le meme port

// ca faut que je revoie l architecture de comment je stocke mes donnees depuis parent ou enfant
//     check pour les doublons de roots
//     check pour les doublons d index
//     check pour les doublons de location path

// OK  check pour invalid IP: 0.0.0.0 à 255.255.255.255
// OK  check pour invalid port: Les ports vont de 0 à 65535, mais tous ne sont pas recommandés

// le double listen doit etre possible -> donc vector

// root dans location / {}

//     Quand vous définissez root à l'intérieur de location / {}, cela remplace le root défini au-dessus.
//     Le index de location / {} doit être relatif au root défini dedans.