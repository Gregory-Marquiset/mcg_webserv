#include "../../includes/configFile/LocationBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

LocationBlock::LocationBlock() {};

LocationBlock::~LocationBlock() {};

/* ================= SETTERS ======================== */

void LocationBlock::setPath(std::string path) {
    this->_path = path.substr(9);
}

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

std::string LocationBlock::getPath() const {
    return (this->_path);
}

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