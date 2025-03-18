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

/* ================= HELPERS ======================== */

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

void ServerBlock::addLocationBlock(const LocationBlock& location) {
    this->_location.push_back(location);
}

/* ================= SET OBJECT SERVERBLOCK ======================== */

std::vector<ServerBlock> ServerBlock::getAllServerBlocks(RecupBlockContent rawConfig) {
    std::vector<ServerBlock> cleanServers;
    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    for (std::vector<Block>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
        if (it->getName() == "server") {
            ServerBlock oneServerBlock;
            std::map<std::string, std::string> directive = it->getDirective();

            if (directive.find("listen") != directive.end()) {
                oneServerBlock.setListen(myStoi(directive["listen"]));
            }
            if (directive.find("server_name") != directive.end()) {
                oneServerBlock.setServerName(directive["server_name"]);
            }

            const std::vector<Block>& locations = it->getChildBlock();
            for (std::vector<Block>::const_iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {
                LocationBlock locBlock;
                std::map<std::string, std::string> locationDirective = itLocation->getDirective();

                if (locationDirective.find("root") != locationDirective.end()) {
                    locBlock.setRoot(locationDirective["root"]);
                }
                if (locationDirective.find("index") != locationDirective.end()) {
                    locBlock.setIndex(locationDirective["index"]);
                }
                if (locationDirective.find("allow_methods") != locationDirective.end()) {
                    locBlock.setAllowMethods(locationDirective["allow_methods"]);
                    std::cout << "hihihihi\n";
                }
                if (locationDirective.find("cgi_extension") != locationDirective.end()) {
                    locBlock.setCgiExtension(locationDirective["cgi_extension"]);
                }

                locBlock.setPath(itLocation->getName());
                oneServerBlock.addLocationBlock(locBlock);
            }

            cleanServers.push_back(oneServerBlock);
        }
    }
    return cleanServers;
}