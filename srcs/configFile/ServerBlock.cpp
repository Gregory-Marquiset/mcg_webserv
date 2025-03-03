#include "../../includes/configFile/ServerBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ServerBlock::ServerBlock() {};

ServerBlock::~ServerBlock() {};

/* ================= SETTERS ======================== */

void ServerBlock::setServer(std::string server) {
    this->_server = server;
}

void ServerBlock::setListen(int listen) {
    this->_listen = listen;
}

void ServerBlock::setServerName(std::string serverName) {
    this->_serverName = serverName;
}

void ServerBlock::setLocation(std::string location) {
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

/* ================= GETTERS ======================== */

std::string ServerBlock::getServer() const {
    return (this->_server);
}

int ServerBlock::getListen() const {
    return (this->_listen);
}

std::string ServerBlock::getServerName() const {
    return (this->_serverName);
}

std::string ServerBlock::getLocation() const {
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

/* ================= SET OBJECT SERVERBLOCK ======================== */

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

void ServerBlock::setServerBlock(RecupBlockContent rawConfig) {
    
    std::vector<Block> serverBlocks = rawConfig.getServerBlocks();

    std::vector<Block>::const_iterator it;

    /* assignation des directives parentes */
    int i = 0;
    for (it = serverBlocks.begin(); it != serverBlocks.end(); it++) {
        
        if (it->getName() == "server") {
            std::cout << i << std::endl;
            i++;
            std::map<std::string, std::string> parentDirectives = it->getDirective();

            if (parentDirectives.find("listen") != parentDirectives.end()) {
                this->setListen(myStoi(parentDirectives["listen"]));
                std::cout << myStoi(parentDirectives["listen"]) << std::endl;
            }
    
            if (parentDirectives.find("server_name") != parentDirectives.end()) {
                this->setServerName(parentDirectives["server_name"]);
            }
    
            if (parentDirectives.find("location") != parentDirectives.end()) {
                this->setServerName(parentDirectives["location"]);
            }
    
            /* assignation des directives enfants */
    
            const std::vector<Block>& children = it->getChildBlock();            
            
            std::vector<Block>::const_iterator itChild;
            
            for (itChild = children.begin(); itChild != children.end(); itChild++) {
            
                std::map<std::string, std::string> childDirectives = itChild->getDirective();
    
                if (childDirectives.find("root") != childDirectives.end()) {
                    this->setRoot(childDirectives["root"]);
                }
                
                if (childDirectives.find("index") != childDirectives.end()) {
                    this->setIndex(childDirectives["index"]);
                }
    
                if (childDirectives.find("allow_methods") != childDirectives.end()) {
                    this->setAllowMethods(childDirectives["allow_methods"]);
                }
                
                if (childDirectives.find("cgi_extension") != childDirectives.end()) {
                    this->setCgiExtension(childDirectives["cgi_extension"]);
                }
            }
        }
    }
}

std::vector<ServerBlock> ServerBlock::getAllServerBlocks(RecupBlockContent rawConfig) {
    
    std::vector<ServerBlock> cleanServers;

    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    for (std::vector<Block>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); it++) {
        
        if (it->getName() == "server") {

            ServerBlock oneServerBlock;

            std::map<std::string, std::string> parentDirectives = it->getDirective();
    
            if (parentDirectives.find("listen") != parentDirectives.end()) {
                int port = myStoi(parentDirectives["listen"]);
                oneServerBlock.setListen(port); 
            }
    
            if (parentDirectives.find("server_name") != parentDirectives.end()) {
                oneServerBlock.setServerName(parentDirectives["server_name"]);
            }
    
            if (parentDirectives.find("location") != parentDirectives.end()) {
                oneServerBlock.setServerName(parentDirectives["location"]);
            }
    
            const std::vector<Block>& children = it->getChildBlock();
            for (std::vector<Block>::const_iterator itChild = children.begin(); itChild != children.end(); ++itChild) {
                std::map<std::string, std::string> childDirectives = itChild->getDirective();
    
                if (childDirectives.find("root") != childDirectives.end()) {
                    oneServerBlock.setRoot(childDirectives["root"]);
                }
                
                if (childDirectives.find("index") != childDirectives.end()) {
                    oneServerBlock.setIndex(childDirectives["index"]);
                }
    
                if (childDirectives.find("allow_methods") != childDirectives.end()) {
                    oneServerBlock.setAllowMethods(childDirectives["allow_methods"]);
                }
                
                if (childDirectives.find("cgi_extension") != childDirectives.end()) {
                    oneServerBlock.setCgiExtension(childDirectives["cgi_extension"]);
                }
            }

            cleanServers.push_back(oneServerBlock);
        }
    }

    return cleanServers; 
}

void ServerBlock::printServerBlockInfo() const {
    std::cout << "===== Server Info =====" << std::endl;
    std::cout << "Server Name: " << this->getServerName() << std::endl;
    std::cout << "Port: " << this->getListen() << std::endl;
    std::cout << "Root: " << this->getRoot() << std::endl;
    std::cout << "Index: " << this->getIndex() << std::endl;
    std::cout << "=======================\n" << std::endl;
}