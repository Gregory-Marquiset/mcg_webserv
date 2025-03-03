#include "../../includes/configFile/LocationBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

LocationBlock::LocationBlock() {};

LocationBlock::~LocationBlock() {};

/* ================= SETTERS ======================== */

void LocationBlock::setRoot(std::string root) {
    this->_root = root;
}

void LocationBlock::setIndex(std::string index) {
    this->_index = index;
}

void LocationBlock::setAllowMethods(std::string allowMethods) {
    this->_allowMethods = allowMethods;
}

void LocationBlock::setCgiExtension(std::string cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

/* ================= GETTERS ======================== */

std::string LocationBlock::getRoot() const {
    return (this->_root);
}

std::string LocationBlock::getIndex() const {
    return (this->_index);
}

std::string LocationBlock::getAllowMethods() const {
    return (this->_allowMethods);
}

std::string LocationBlock::getCgiExtension() const {
    return (this->_cgiExtension);
}

/* ================= SET OBJECT LOCATIONBLOCK ======================== */


// en faite j ai capte location est un child donc revoir le code 
std::vector<LocationBlock> LocationBlock::getAllLocationBlocks(RecupBlockContent rawConfig) {
    
    std::vector<LocationBlock> cleanLocations;

    std::vector<Block> allBlocks = rawConfig.getServerBlocks();

    int i = 0;
    for (std::vector<Block>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); it++) {
        
        if (it->getName() == "server") {
            std::cout << "server\n";
            std::map<std::string, std::string> parentDirectives = it->getDirective();
            
            if (parentDirectives.find("location") != parentDirectives.end()) {
                
                LocationBlock oneLocationBlock;
                std::cout << i << std::endl;
                i++;

                std::map<std::string, std::string> locationDirectives = it->getDirective();
        
                if (locationDirectives.find("root") != locationDirectives.end()) {
                    oneLocationBlock.setRoot(locationDirectives["root"]);
                }
                
                if (locationDirectives.find("index") != locationDirectives.end()) {
                    oneLocationBlock.setIndex(locationDirectives["index"]);
                }

                if (locationDirectives.find("allow_methods") != locationDirectives.end()) {
                    oneLocationBlock.setAllowMethods(locationDirectives["allow_methods"]);
                }
                
                if (locationDirectives.find("cgi_extension") != locationDirectives.end()) {
                    oneLocationBlock.setCgiExtension(locationDirectives["cgi_extension"]);
                }
            
                cleanLocations.push_back(oneLocationBlock);
            }
        }
    }
    std::cout << "coucou" << std::endl;
    return cleanLocations; 
}

void LocationBlock::printLocationBlockInfo() const {
    std::cout << "===== Location Info =====" << std::endl;
    std::cout << "Root: " << this->getRoot() << std::endl;
    std::cout << "Index: " << this->getIndex() << std::endl;
    std::cout << "=======================\n" << std::endl;
}