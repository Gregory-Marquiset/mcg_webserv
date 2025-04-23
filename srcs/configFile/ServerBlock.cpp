#include "../../includes/configFile/ServerBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

ServerBlock::ServerBlock() {};

ServerBlock::~ServerBlock() {};

/* ================= SETTERS ======================== */

void ServerBlock::setServer(std::string server) {
    this->_server = server;
}

void ServerBlock::setPort(std::vector<int> port) {
    this->_port = port;
}

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

std::vector<int> ServerBlock::getPort() const {
    return (this->_port);
}

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

void ServerBlock::addLocationBlock(const LocationBlock& location) {
    this->_location.push_back(location);
}

void ServerBlock::addPort(const int& port) {
    this->_port.push_back(port);
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
        throw (std::invalid_argument("Error: there is a Block with no root available"));
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
        throw (std::invalid_argument("Error: there is a Block with no index available"));
    }
}

void ServerBlock::portCheck() {

    if (this->getPort().empty()) {
        throw (std::invalid_argument("Error: no port found"));
    }
    
    for (size_t i = 0; i < this->getPort().size(); ++i) {
        for (size_t j = i + 1; j < this->getPort().size(); ++j) {
            if (this->getPort()[i] == this->getPort()[j]) {
                throw (std::invalid_argument("Error: same ports found in one of the servers"));
            }
        }
    }
}

/* SETTER DU SERVER BLOCK SANS LOCATION BLOCK IMBRIQUEE */

void ServerBlock::caseWithNoLocationBlockEmbeded(ServerBlock& oneServerBlock, std::multimap<std::string, std::string> directive, HostHandler& host) {

    int flag = 0;

    if (directive.empty()) {
        throw (std::invalid_argument("Error: Provide some directives in .conf"));
    }

    // root
    if (directive.count("root") == 0) {
        oneServerBlock.setRoot("");
    } else if (directive.count("root") == 1) {
        std::multimap<std::string, std::string>::iterator itRoot = directive.find("root");
        oneServerBlock.setRoot(itRoot->second);
    } else {
        throw (std::invalid_argument("Error: Too many roots"));
    }

    // index

    if (directive.count("index") == 0) {
        oneServerBlock.setIndex("");
    } else if (directive.count("index") == 1) {
        std::multimap<std::string, std::string>::iterator itIndex = directive.find("index");
        oneServerBlock.setIndex(itIndex->second);
    } else {
        throw (std::invalid_argument("Error: Too many indexes"));
    }

    // listen
    if (directive.count("listen") == 0) {
        throw (std::invalid_argument("Error: Provide a port where to listen"));
    } else {
        for (std::multimap<std::string, std::string>::iterator itPort = directive.lower_bound("listen"); itPort != directive.upper_bound("listen"); ++itPort) {
            host.checkListenFormat(itPort->second, oneServerBlock);
            flag = 1;
        }
    }

    if (directive.count("server_name") == 0) {

        host.getHostName().push_back("localhost");
        flag = 1;
    }
    else if (directive.count("server_name") == 1) {
        std::multimap<std::string, std::string>::iterator itServerName = directive.find("server_name");

        std::stringstream ss(itServerName->second);
        std::string name;
        while (ss >> name) {
            host.getHostName().push_back(name);
            flag = 1;
        }
 
    } else {
        throw (std::invalid_argument("Error: Too many server_names"));
    }

    if (flag == 1)
        oneServerBlock._host.push_back(host);


    // body size
    if (directive.count("client_max_body_size") == 0) {
        oneServerBlock.setClientMaxBodySize("1M");
    } else if (directive.count("client_max_body_size") == 1) {
        std::multimap<std::string, std::string>::iterator itBodySize = directive.find("client_max_body_size");
        oneServerBlock.setClientMaxBodySize(itBodySize->second);
    } else {
        throw (std::invalid_argument("Error: too many client_max_body_sizes"));
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
        throw (std::invalid_argument("Error: too many allow_methods"));
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
        throw (std::invalid_argument("Error: too many autoindexes"));
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
            throw (std::invalid_argument("Error: redirection format is not correct"));
        }
        oneServerBlock.setRedirection(res);
    }
    else {
        throw (std::invalid_argument("Error: too many returns"));
    }
}

// /* SETTER DU SERVER BLOCK AVEC LOCATION BLOCK IMBRIQUEE */

void ServerBlock::caseWithLocationBlockEmbeded(ServerBlock& oneServerBlock, LocationBlock& locBlock, std::multimap<std::string, std::string> directive) {

    if (directive.empty()) {
        throw (std::invalid_argument("Error: Provide some directives in .conf"));
    }

    // root
    if (directive.count("root") == 0) {
        locBlock.setRoot("");
    } else if (directive.count("root") == 1) {
        std::multimap<std::string, std::string>::iterator itRoot = directive.find("root");
        locBlock.setRoot(itRoot->second);
    } else {
        throw (std::invalid_argument("Error: Too many roots"));
    }

    // index
    if (directive.count("index") == 0 && oneServerBlock.getIndex() == "") {
        throw (std::invalid_argument("Error: Please provide an index"));
    } else if (directive.count("index") == 0 && oneServerBlock.getIndex() != "") {
        locBlock.setIndex(oneServerBlock.getIndex());
    } else if (directive.count("index") == 1) {
        std::multimap<std::string, std::string>::iterator itIndex = directive.find("index");
        locBlock.setIndex(itIndex->second);
    } else {
        throw (std::invalid_argument("Error: Too many indexes"));
    }

    // body size
    if (directive.count("client_max_body_size") == 0) {
        locBlock.setClientMaxBodySize(oneServerBlock.getClientMaxBodySize());
    } else if (directive.count("client_max_body_size") == 1) {
        std::multimap<std::string, std::string>::iterator itBodySize = directive.find("client_max_body_size");
        locBlock.setClientMaxBodySize(itBodySize->second);
    } else {
        throw (std::invalid_argument("Error: too many client_max_body_sizes"));
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
        throw (std::invalid_argument("Error: too many allow_methods"));
    }

    // auto index
    if (directive.count("autoindex") == 0) {
        locBlock.setAutoIndex(oneServerBlock.getAutoIndex());
    } else if (directive.count("autoindex") == 1) {
        std::multimap<std::string, std::string>::iterator itAutoIndex = directive.find("autoindex");
        locBlock.setAutoIndex(itAutoIndex->second);
    }
    else {
        throw (std::invalid_argument("Error: too many autoindexes"));
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
            throw (std::invalid_argument("Error: redirection format is not correct"));
        }
        locBlock.setRedirection(res);
    }
    else {
        throw (std::invalid_argument("Error: too many returns"));
    }
}

/* MAIN FUNCTION TO CREATE SERVER BLOCKS */

std::vector<ServerBlock> ServerBlock::createAllServerBlocks(RecupBlockContent rawConfig) {
    
    std::vector<ServerBlock> cleanServers;

    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    for (std::vector<Block>::iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
                
        if (it->getName() == "server") {

            ServerBlock oneServerBlock;
            HostHandler host;

            std::multimap<std::string, std::string> directive = it->getDirective();

            std::vector<Block> locations = it->getChildBlock();
            if (locations.empty()) {
                caseWithNoLocationBlockEmbeded(oneServerBlock, directive, host);

                LocationBlock locBlock;

                locBlock.setPathSpecial("/");
                caseWithLocationBlockEmbeded(oneServerBlock, locBlock, directive);
                oneServerBlock.addLocationBlock(locBlock);
            }
            else {
                int flag = 0;
                
                for (std::vector<Block>::iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {
                    
                    if (itLocation->getName() == "location /") {
                        flag = 1;
                    }
                }
                
                caseWithNoLocationBlockEmbeded(oneServerBlock, directive, host);
                
                if (flag == 0) {
                    LocationBlock locBlock;
                    
                    locBlock.setPathSpecial("/");
                    caseWithLocationBlockEmbeded(oneServerBlock, locBlock, directive);
                    oneServerBlock.addLocationBlock(locBlock);
                }
                 
                for (std::vector<Block>::iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {

                    LocationBlock locBlock;

                    locBlock.setPath(itLocation->getName());

                    for (size_t iBlock = 0; iBlock < oneServerBlock.getLocationBlock().size(); ++iBlock) {
                        if (locBlock.getPath() == oneServerBlock.getLocationBlock()[iBlock].getPath()) {
                            throw (std::invalid_argument("Error: identical location path"));
                        }
                    }
                    std::multimap<std::string, std::string> locDirective = itLocation->getDirective();
                    caseWithLocationBlockEmbeded(oneServerBlock, locBlock, locDirective);
                    oneServerBlock.addLocationBlock(locBlock);
                }
            }
            oneServerBlock.rootCheck();
            oneServerBlock.indexCheck();
            oneServerBlock.portCheck();
            cleanServers.push_back(oneServerBlock);
        }
        if (cleanServers.empty()) {
            throw (std::invalid_argument("Error: Something went wrong in .conf - file might be empty"));
        }
    }
    return (cleanServers);
}