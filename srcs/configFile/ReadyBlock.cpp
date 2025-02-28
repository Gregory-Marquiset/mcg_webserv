#include "../../includes/configFile/ReadyBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ReadyBlock::ReadyBlock() {};

ReadyBlock::~ReadyBlock() {};

/* ================= SETTERS ======================== */

void ReadyBlock::setServer(std::string server) {
    this->_server = server;
}

void ReadyBlock::setListen(int listen) {
    this->_listen = listen;
}

void ReadyBlock::setServerName(std::string serverName) {
    this->_serverName = serverName;
}

void ReadyBlock::setLocation(std::string location) {
    this->_location = location;
}

void ReadyBlock::setRoot(std::string root) {
    this->_root = root;
}

void ReadyBlock::setIndex(std::string index) {
    this->_index = index;
}

void ReadyBlock::setAllowMethods(std::string allowMethods) {
    this->_allowMethods = allowMethods;
}

void ReadyBlock::setCgiExtension(std::string cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

/* ================= GETTERS ======================== */

std::string ReadyBlock::getServer() const {
    return (this->_server);
}

int ReadyBlock::getListen() const {
    return (this->_listen);
}

std::string ReadyBlock::getServerName() const {
    return (this->_serverName);
}

std::string ReadyBlock::getLocation() const {
    return (this->_location);
}

std::string ReadyBlock::getRoot() const {
    return (this->_root);
}

std::string ReadyBlock::getIndex() const {
    return (this->_index);
}

std::string ReadyBlock::getAllowMethods() const {
    return (this->_allowMethods);
}

std::string ReadyBlock::getCgiExtension() const {
    return (this->_cgiExtension);
}

/* ================= SET OBJECT READYBLOCK (SERVER BLOCK) ======================== */

void ReadyBlock::setReadyBlock(RecupBlockContent rawConfig) {

    std::vector<Block> serverBlocks = rawConfig.getServerBlocks();

    std::vector<Block>::const_iterator it;

    /* assignation des directives parentes */
    
    for (it = serverBlocks.begin(); it != serverBlocks.end(); it++) {

        // s assurer de combien il y a de blocks 
        
        // if (parentDirectives.find("server")) {

        //     std::map<std::string, std::string> parentDirectives = it->getDirective();
    
        //     if (parentDirectives.find("listen") != parentDirectives.end()) {
        //         this->setListen(myStoi(parentDirectives["listen"]));
        //     }
    
        //     if (parentDirectives.find("server_name") != parentDirectives.end()) {
        //         this->setServerName(parentDirectives["server_name"]);
        //     }
    
        //     if (parentDirectives.find("location") != parentDirectives.end()) {
        //         this->setServerName(parentDirectives["location"]);
        //     }
    
        //     /* assignation des directives enfants */
    
        //     const std::vector<Block>& children = it->getChildBlock();            
            
        //     std::vector<Block>::const_iterator itChild;
            
        //     for (itChild = children.begin(); itChild != children.end(); itChild++) {
            
        //         std::map<std::string, std::string> childDirectives = itChild->getDirective();
    
        //         if (childDirectives.find("root") != childDirectives.end()) {
        //             this->setRoot(childDirectives["root"]);
        //         }
                
        //         if (childDirectives.find("index") != childDirectives.end()) {
        //             this->setIndex(childDirectives["index"]);
        //         }
    
        //         if (childDirectives.find("allow_methods") != childDirectives.end()) {
        //             this->setAllowMethods(childDirectives["allow_methods"]);
        //         }
                
        //         if (childDirectives.find("cgi_extension") != childDirectives.end()) {
        //             this->setCgiExtension(childDirectives["cgi_extension"]);
        //         }
        //     }
        // }
        
    }
}
