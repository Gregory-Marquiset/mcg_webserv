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

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return (i);
}

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

void ServerBlock::caseWithNoLocationBlockEmbeded(ServerBlock& oneServerBlock, std::multimap<std::string, std::string> directive, HostHandler& host) {

    // HostHandler host;
    int flag = 0;

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
    if (directive.count("listen") == 0) {
        std::cerr << "Error: Provide a port where to listen" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        for (std::multimap<std::string, std::string>::iterator itPort = directive.lower_bound("listen"); itPort != directive.upper_bound("listen"); ++itPort) {
            HostHandler host;
            host.checkListenFormat(itPort->second, oneServerBlock);
            // oneServerBlock._host.push_back(host);
            flag = 1;
        }
    }

    if (directive.count("server_name") == 0) {
        
        // HostHandler host;

        host.setHostName("localhost");
        // oneServerBlock._host.push_back(host);
        flag = 1;
    }
    else if (directive.count("server_name") == 1) {
        std::multimap<std::string, std::string>::iterator itServerName = directive.find("server_name");

        // HostHandler host;

        host.filter(itServerName->second);
        if (host.getHostFormat() == 1) {
            host.parseIp(itServerName->second);
            flag = 1;
            // oneServerBlock._host.push_back(host);
        }
        else {

            std::stringstream ss(itServerName->second);
            std::string name;
            while (ss >> name) {
                host.setHostName(name);
                // oneServerBlock._host.push_back(host);
                flag = 1;
            }
        }
    } else {
        std::cerr << "Error: Too many server_names" << std::endl;
        exit(EXIT_FAILURE);
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

// donc ici faire check supplemetaires pour regarder si le servers a certaines directives ou non 

void ServerBlock::caseWithLocationBlockEmbeded(ServerBlock& oneServerBlock, LocationBlock& locBlock, std::multimap<std::string, std::string> directive, HostHandler& host) {

    (void)host;

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
    if (directive.count("index") == 0 && oneServerBlock.getIndex() == "") {
        std::cerr << "Error: Please provide an index" << std::endl;
        exit(EXIT_FAILURE);
    } else if (directive.count("index") == 0 && oneServerBlock.getIndex() != "") {
        locBlock.setIndex(oneServerBlock.getIndex());
    } else if (directive.count("index") == 1) {
        std::multimap<std::string, std::string>::iterator itIndex = directive.find("index");
        locBlock.setIndex(itIndex->second);
    } else {
        std::cerr << "Error: Too many indexes" << std::endl;
        exit(EXIT_FAILURE);
    }

    // body size
    if (directive.count("client_max_body_size") == 0) {
        locBlock.setClientMaxBodySize(oneServerBlock.getClientMaxBodySize());
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
        locBlock.setAutoIndex(oneServerBlock.getAutoIndex());
    } else if (directive.count("autoindex") == 1) {
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

// faire une fonction dans le cas ou pas de location /
// faire un tour d tous les paths, si pas de / alors:
// creer location / bloc
// index, autoindex, client_mx_size_body heritent des directives servers si non redefinis -> don a faire valoir pour tous les les locs blocks

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
                caseWithLocationBlockEmbeded(oneServerBlock, locBlock, directive, host);
                oneServerBlock.addLocationBlock(locBlock);
            }
            else {
                int flag = 0;
                
                for (std::vector<Block>::iterator itLocation = locations.begin(); itLocation != locations.end(); ++itLocation) {
                    
                    // std::cout << "name = " << itLocation->getName() << std::endl;
                    if (itLocation->getName() == "location /") {
                        // std::cout << "found\n";
                        flag = 1;
                    }
                }
                
                caseWithNoLocationBlockEmbeded(oneServerBlock, directive, host);
                
                if (flag == 0) {
                    LocationBlock locBlock;
                    
                    locBlock.setPathSpecial("/");
                    caseWithLocationBlockEmbeded(oneServerBlock, locBlock, directive, host);
                    oneServerBlock.addLocationBlock(locBlock);
                }
                 
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
                    caseWithLocationBlockEmbeded(oneServerBlock, locBlock, locDirective, host);
                    oneServerBlock.addLocationBlock(locBlock);
                }
            }
            oneServerBlock.rootCheck();
            oneServerBlock.indexCheck();
            cleanServers.push_back(oneServerBlock);
        }
        if (cleanServers.empty()) {
            std::cerr << "Error: No server in .conf" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return (cleanServers);
}