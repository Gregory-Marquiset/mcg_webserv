#include "../../../includes/assignValues/ConfigData.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ConfigData::ConfigData() {};

ConfigData::~ConfigData() {};

/* ================= SETTERS ======================== */

void ConfigData::setServer(std::string server) {
    this->_server = server;
}

void ConfigData::setListen(int listen) {
    this->_listen = listen;
}

void ConfigData::setServerName(std::string serverName) {
    this->_serverName = serverName;
}

void ConfigData::setLocation(std::string location) {
    this->_location = location;
}

void ConfigData::setRoot(std::string root) {
    this->_root = root;
}

void ConfigData::setIndex(std::string index) {
    this->_index = index;
}

void ConfigData::setAllowMethods(std::string allowMethods) {
    this->_allowMethods = allowMethods;
}

void ConfigData::setCgiExtension(std::string cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

/* ================= GETTERS ======================== */

std::string ConfigData::getServer() const {
    return (this->_server);
}

int ConfigData::getListen() const {
    return (this->_listen);
}

std::string ConfigData::getServerName() const {
    return (this->_serverName);
}

std::string ConfigData::getLocation() const {
    return (this->_location);
}

std::string ConfigData::getRoot() const {
    return (this->_root);
}

std::string ConfigData::getIndex() const {
    return (this->_index);
}

std::string ConfigData::getAllowMethods() const {
    return (this->_allowMethods);
}

std::string ConfigData::getCgiExtension() const {
    return (this->_cgiExtension);
}

/* ================= SET OBJECT CONFIG DATA ======================== */

void ConfigData::setConfigData(Parse rawConfig) {

    std::vector<Block> blocks = rawConfig.getConfigBlocks();

    std::vector<Block>::const_iterator it;

    /* assignation des directives parentes */
    
    for (it = blocks.begin(); it != blocks.end(); it++) {
        
        std::map<std::string, std::string> parentDirectives = it->getDirective();

        if (parentDirectives.find("listen") != parentDirectives.end()) {
            this->setListen(myStoi(parentDirectives["listen"]));
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
