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

void ServerBlock::setHost(std::vector<HostHandler> host) {
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

void ServerBlock::setAutoIndex(std::string status) {
    this->_auto_index = status;
}

void ServerBlock::setRedirection(std::vector<std::string> redirection) {
    this->_redirection = redirection;
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

std::vector<HostHandler> ServerBlock::getHost() const {
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

std::string ServerBlock::getAutoIndex() const {
    return (this->_auto_index);
}

std::vector<std::string> ServerBlock::getRedirection() const {
    return (this->_redirection);
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

    // if (directive.count("listen") == 0) {
    //    oneServerBlock.setPort(-1); nop changer ca 
    // } else {
    // for (std::multimap<std::string, std::string>::iterator itPort = directive.lower_bound("listen"); itPort != directive.upper_bound("listen"); ++itPort) {
        
    //     if (std::atoi((itPort->second).c_str()) < 0 || std::atoi((itPort->second).c_str()) > 65535) {
    //         std::cerr << "Error: Invalid Port Number" << std::endl;
    //         exit(EXIT_FAILURE);
    //     }
    //     oneServerBlock._port.push_back(std::atoi((itPort->second).c_str()));
    // }
    // }

    // listen
    if (directive.count("listen") != 1) {
        std::cerr << "Error: Provide ONE port per each server" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (directive.count("listen") == 1) {
            std::multimap<std::string, std::string>::iterator itPort = directive.find("listen");
            if (std::atoi((itPort->second).c_str()) < 0 || std::atoi((itPort->second).c_str()) > 65535) {
                std::cerr << "Error: Invalid Port Number" << std::endl;
                exit(EXIT_FAILURE);
            }
            oneServerBlock.setPort(std::atoi((itPort->second).c_str()));
    }

    if (directive.count("server_name") == 0) {
        HostHandler host;

        host.setHostName("127.0.0.1");
        oneServerBlock._host.push_back(host);
    }
    else if (directive.count("server_name") == 1) {
        std::multimap<std::string, std::string>::iterator itServerName = directive.find("server_name");

        HostHandler host;
        
        host.filter(itServerName->second);
        if (host.getHostFormat() == 1) {
            host.parseIp(itServerName->second);
            oneServerBlock._host.push_back(host);
        }
        else {

            std::stringstream ss(itServerName->second);
            std::string name;
            while (ss >> name) {
                host.setHostName(name);
                oneServerBlock._host.push_back(host);
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

    // auto index
    if (directive.count("autoindex") == 0) {
        oneServerBlock.setAutoIndex("off");
    }
    else if (directive.count("autoindex") == 1) {
        std::multimap<std::string, std::string>::iterator itAutoIndex = directive.find("autoindex");
        oneServerBlock.setAutoIndex(itAutoIndex->second);
    }
    else {
        std::cerr << "Error: too many autoindexes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // redirection
    if (directive.count("return") == 0) {
        std::vector<std::string> res;
        res.clear();
        oneServerBlock.setRedirection(res);
    }
    else if (directive.count("return") == 1) {
        std::multimap<std::string, std::string>::iterator itRedir = directive.find("return");

        std::stringstream ss(itRedir->second);
        std::vector<std::string> res;
        std::string word;
        while (ss >> word) {
            res.push_back(word);
        }
        if (res.size() != 2) {
            std::cerr << "Error: redirection format is not correct" << std::endl;
            exit(EXIT_FAILURE);
        }
        oneServerBlock.setRedirection(res);
    }
    else {
        std::cerr << "Error: too many returns" << std::endl;
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

    // auto index
    if (directive.count("autoindex") == 0) {
        locBlock.setAutoIndex("off");
    }
    else if (directive.count("autoindex") == 1) {
        std::multimap<std::string, std::string>::iterator itAutoIndex = directive.find("autoindex");
        locBlock.setAutoIndex(itAutoIndex->second);
    }
    else {
        std::cerr << "Error: too many autoindexes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // redirection
    if (directive.count("return") == 0) {
        std::vector<std::string> res;
        res.clear();
        locBlock.setRedirection(res);
    }
    else if (directive.count("return") == 1) {
        std::multimap<std::string, std::string>::iterator itRedir = directive.find("return");

        std::stringstream ss(itRedir->second);
        std::vector<std::string> res;
        std::string word;
        while (ss >> word) {
            res.push_back(word);
        }
        
        if (res.size() != 2) {
            std::cerr << "Error: redirection format is not correct" << std::endl;
            exit(EXIT_FAILURE);
        }
        locBlock.setRedirection(res);
    }
    else {
        std::cerr << "Error: too many returns" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* MAIN FUNCTION TO CREATE SERVER BLOCKS */

std::vector<ServerBlock> ServerBlock::createAllServerBlocks(RecupBlockContent rawConfig) {
    
    std::vector<ServerBlock> cleanServers;
    
    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    for (std::vector<Block>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
                
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
            cleanServers.push_back(oneServerBlock);
        }

        if (cleanServers.empty()) {
            std::cerr << "Error: No server in .conf" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return (cleanServers);
}