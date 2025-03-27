#include "../../includes/configFile/ServerBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ServerBlock::ServerBlock() {};

ServerBlock::~ServerBlock() {};

/* ================= SETTERS ======================== */

void ServerBlock::setServer(std::string server) {
    this->_server = server;
}

void ServerBlock::setPort(int port) {
    this->_port = port;
}

// void ServerBlock::setServerName(std::string serverName) {
//     this->_serverName = serverName;
// }

void ServerBlock::setHost(const HostHandler& host) {
    this->_host = host;
}

void ServerBlock::setRoot(std::string root) {
    this->_root = root;
}

void ServerBlock::setIndex(std::string index) {
    this->_index = index;
}

void ServerBlock::setLocation(std::vector<LocationBlock> location) {
    this->_location = location;
}

void ServerBlock::setAllowMethods(std::vector<std::string> allowMethods) {
    this->_allowMethods = allowMethods;
}

void ServerBlock::setCgiExtension(std::vector<CgiHandler> cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

void ServerBlock::setClientMaxBodySize(std::string client_max_body_size) {
    this->_client_max_body_size = client_max_body_size;
}

/* ================= GETTERS ======================== */

std::string ServerBlock::getServer() const {
    return (this->_server);
}

int ServerBlock::getPort() const {
    return (this->_port);
}

// std::string ServerBlock::getServerName() const {
//     return (this->_serverName);
// }

HostHandler ServerBlock::getHost() const {
    return (this->_host);
}

std::string ServerBlock::getRoot() const {
    return (this->_root);
}

std::string ServerBlock::getIndex() const {
    return (this->_index);
}

std::vector<LocationBlock> ServerBlock::getLocationBlock() const {
    return (this->_location);
}

std::vector<std::string> ServerBlock::getAllowMethods() const {
    return (this->_allowMethods);
}

std::vector<CgiHandler> ServerBlock::getCgiExtension() const {
    return (this->_cgiExtension);
}

std::string ServerBlock::getClientMaxBodySize() const {
    return (this->_client_max_body_size);
}

/* ================= HELPERS & CHECKS ======================== */

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

void ServerBlock::addLocationBlock(const LocationBlock& location) {
    this->_location.push_back(location);
}

void ServerBlock::rootCheck() {
    
    int noRootInServer = 0;
    int noRootInLoc = 0;

    if (this->getRoot().empty()) {
        noRootInServer = 1;
    }
    for (size_t i = 0; i < this->getLocationBlock().size(); ++i) {
        if (this->getLocationBlock()[i].getRoot().empty())
            noRootInLoc = 1;
    }
    if (noRootInServer == 1 && noRootInLoc == 1) {
        std::cerr << "Error: there is a Block with no root available" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ServerBlock::indexCheck() {
    
    int noIndexInServer = 0;
    int noIndexInLoc = 0;

    if (this->getIndex().empty()) {
        noIndexInServer = 1;
    }
    for (size_t i = 0; i < this->getLocationBlock().size(); ++i) {
        if (this->getLocationBlock()[i].getIndex().empty())
            noIndexInLoc = 1;
    }
    if (noIndexInServer == 1 && noIndexInLoc == 1) {
        std::cerr << "Error: there is a Block with no index available" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* SETTER DU SERVER BLOCK SANS LOCATION BLOCK IMBRIQUEE */

void ServerBlock::caseWithNoLocationBlockEmbeded(ServerBlock& oneServerBlock, std::multimap<std::string, std::string> directive) {
    
    if (directive.empty()) {
        std::cerr << "Error: Provide some directives in .conf" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // root
    if (directive.count("root") == 0) {
        oneServerBlock.setRoot("");
    } else if (directive.count("root") == 1) {
        std::multimap<std::string, std::string>::iterator itRoot = directive.find("root");
        oneServerBlock.setRoot(itRoot->second);
    } else {
        std::cerr << "Error: Too many roots" << std::endl;
        exit(EXIT_FAILURE);
    }

    // index
    if (directive.count("index") == 0) {
        oneServerBlock.setIndex("");
    } else if (directive.count("index") == 1) {
        std::multimap<std::string, std::string>::iterator itIndex = directive.find("index");
        oneServerBlock.setIndex(itIndex->second);
    } else {
        std::cerr << "Error: Too many indexes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // listen
    if (directive.count("listen") != 1) {
        std::cerr << "Error: Provide ONE port per each server" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (directive.count("listen") == 1) {
        
        HostHandler host;
        
        std::multimap<std::string, std::string>::iterator itPort = directive.find("listen");
        host.checkListenFormat(itPort->second, oneServerBlock);
            // if (std::atoi((itPort->second).c_str()) < 0 || std::atoi((itPort->second).c_str()) > 65535) {
            //     std::cerr << "Error: Invalid Port Number" << std::endl;
            //     exit(EXIT_FAILURE);
            // }
            // oneServerBlock.setPort(std::atoi((itPort->second).c_str()));
    }

    if (directive.count("server_name") == 0) {
        
        // HostHandler host;

        oneServerBlock._host.getHostName().push_back("localhost");
        // oneServerBlock._host.push_back(host);
    }
    else if (directive.count("server_name") == 1) {
        std::multimap<std::string, std::string>::iterator itServerName = directive.find("server_name");

        HostHandler host;
        
        host.filter(itServerName->second);
        if (host.getHostFormat() == 1) {
            host.parseIp(itServerName->second);
            // oneServerBlock._host.push_back(host);
        }
        else {

            std::stringstream ss(itServerName->second);
            std::string name;
            while (ss >> name) {
                oneServerBlock._host.getHostName().push_back(name);
                // oneServerBlock._host.push_back(host);
            }
        }
    } else {
        std::cerr << "Error: Too many server_names" << std::endl;
        exit(EXIT_FAILURE);
    }

    // body size
    if (directive.count("client_max_body_size") == 0) {
        oneServerBlock.setClientMaxBodySize("1Mo");
    } else if (directive.count("client_max_body_size") == 1) {
        std::multimap<std::string, std::string>::iterator itBodySize = directive.find("client_max_body_size");
        oneServerBlock.setClientMaxBodySize(itBodySize->second);
    } else {
        std::cerr << "Error: too many client_max_body_sizes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // cgi
    for (std::multimap<std::string, std::string>::iterator itCgi = directive.lower_bound("cgi_extension"); itCgi != directive.upper_bound("cgi_extension"); ++itCgi) {
        CgiHandler cgi;
        cgi.parseCgi(itCgi->second); 
        oneServerBlock._cgiExtension.push_back(cgi);
    }

    // allow methods
    if (directive.count("allow_methods") == 0) {
        oneServerBlock._allowMethods.push_back("GET");
        oneServerBlock._allowMethods.push_back("POST");
        oneServerBlock._allowMethods.push_back("DELETE");
    } else if (directive.count("allow_methods") == 1) {
        std::multimap<std::string, std::string>::iterator itMethods = directive.find("allow_methods");
        std::stringstream ss(itMethods->second);
        std::string method;
        while (ss >> method)
            oneServerBlock._allowMethods.push_back(method);
    } else {
        std::cerr << "Error: too many allow_methods" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// /* SETTER DU SERVER BLOCK AVEC LOCATION BLOCK IMBRIQUEE */

void ServerBlock::caseWithLocationBlockEmbeded(LocationBlock& locBlock, std::multimap<std::string, std::string> directive) {
    
    if (directive.empty()) {
        std::cerr << "Error: Provide some directives in .conf" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // root
    if (directive.count("root") == 0) {
        locBlock.setRoot("");
    } else if (directive.count("root") == 1) {
        std::multimap<std::string, std::string>::iterator itRoot = directive.find("root");
        locBlock.setRoot(itRoot->second);
    } else {
        std::cerr << "Error: Too many roots" << std::endl;
        exit(EXIT_FAILURE);
    }

    // index
    if (directive.count("index") == 0) {
        locBlock.setIndex("");
    } else if (directive.count("index") == 1) {
        std::multimap<std::string, std::string>::iterator itIndex = directive.find("index");
        locBlock.setIndex(itIndex->second);
    } else {
        std::cerr << "Error: Too many indexes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // body size
    if (directive.count("client_max_body_size") == 0) {
        locBlock.setClientMaxBodySize("1Mo");
    } else if (directive.count("client_max_body_size") == 1) {
        std::multimap<std::string, std::string>::iterator itBodySize = directive.find("client_max_body_size");
        locBlock.setClientMaxBodySize(itBodySize->second);
    } else {
        std::cerr << "Error: too many client_max_body_sizes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // cgi
    for (std::multimap<std::string, std::string>::iterator itCgi = directive.lower_bound("cgi_extension"); itCgi != directive.upper_bound("cgi_extension"); ++itCgi) {
        CgiHandler cgi;
        cgi.parseCgi(itCgi->second); 
        locBlock.addCgi(cgi);
    }

    // allow methods
    if (directive.count("allow_methods") == 0) {
        locBlock.addAllowMethod("GET");
        locBlock.addAllowMethod("POST");
        locBlock.addAllowMethod("DELETE");
    } else if (directive.count("allow_methods") == 1) {
        std::multimap<std::string, std::string>::iterator itMethods = directive.find("allow_methods");
        std::stringstream ss(itMethods->second);
        std::string method;
        while (ss >> method)
            locBlock.addAllowMethod(method);
    } else {
        std::cerr << "Error: too many allow_methods" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* MAIN FUNCTION TO CREATE SERVER BLOCKS */

std::vector<ServerBlock> ServerBlock::createAllServerBlocks(RecupBlockContent rawConfig) {
    
    std::vector<ServerBlock> cleanServers;
    
    std::vector<Block> allBlocks = rawConfig.getServerBlocks();
        
    for (std::vector<Block>::iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
        
        if (it->getName() == "server") {

            ServerBlock oneServerBlock;

            std::multimap<std::string, std::string> directive = it->getDirective();

            std::vector<Block> locations = it->getChildBlock();
            if (locations.empty()) {
                caseWithNoLocationBlockEmbeded(oneServerBlock, directive);
            }
            else {
                caseWithNoLocationBlockEmbeded(oneServerBlock, directive);
                for (std::vector<Block>::iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {

                    LocationBlock locBlock;
                    
                    locBlock.setPath(itLocation->getName());
                    
                    for (size_t iBlock = 0; iBlock < oneServerBlock.getLocationBlock().size(); ++iBlock) {
                        if (locBlock.getPath() == oneServerBlock.getLocationBlock()[iBlock].getPath()) {
                            std::cerr << "Error: identical location path" << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    std::multimap<std::string, std::string> locDirective = itLocation->getDirective();
                    caseWithLocationBlockEmbeded(locBlock, locDirective);
                    oneServerBlock.addLocationBlock(locBlock);
                }
            }
            oneServerBlock.rootCheck();
            oneServerBlock.indexCheck();
            cleanServers.push_back(oneServerBlock);
        }
    }
    if (cleanServers.empty()) {
        std::cerr << "Error: in serverblock No server in .conf" << std::endl;
        exit(EXIT_FAILURE);
    }
    return (cleanServers);
}