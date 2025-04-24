#include "../../includes/configFile/LocationBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

LocationBlock::LocationBlock() {};

LocationBlock::~LocationBlock() {};

/* ================= SETTERS ======================== */

void LocationBlock::setPath(std::string path) {

    if (path.empty() || path.size() < 9)
        throw (std::invalid_argument("substr() issue: Error in .conf"));
    else
        this->_path = path.substr(9);
}

void LocationBlock::setPathSpecial(std::string path) {
    this->_path = path;
}

void LocationBlock::setRoot(std::string root) {
    this->_root = root;
}

void LocationBlock::setIndex(std::string index) {
    this->_index = index;
}

void LocationBlock::setAllowMethods(std::vector<std::string> allowMethods) {
    this->_allowMethods = allowMethods;
}

void LocationBlock::setCgiExtension(std::vector<CgiHandler> cgiExtension) {
    this->_cgiExtension = cgiExtension;
}

void LocationBlock::setClientMaxBodySize(std::string client_max_body_size) {
    this->_client_max_body_size = client_max_body_size;
}

void LocationBlock::setAutoIndex(std::string status) {
    this->_auto_index = status;
}

void LocationBlock::setRedirection(std::vector<std::string> redirection) {
    this->_redirection = redirection;
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

std::vector<std::string> LocationBlock::getAllowMethods() const {
    return (this->_allowMethods);
}

std::vector<CgiHandler> LocationBlock::getCgiExtension() const {
    return (this->_cgiExtension);
}

std::string LocationBlock::getClientMaxBodySize() const {
    return (this->_client_max_body_size);
}

std::string LocationBlock::getAutoIndex() const {
    return (this->_auto_index);
}

std::vector<std::string> LocationBlock::getRedirection() const {
    return (this->_redirection);
}

void LocationBlock::addCgi(const CgiHandler& cgi) {
    this->_cgiExtension.push_back(cgi);
}

void LocationBlock::addAllowMethod(const std::string& method) {
    this->_allowMethods.push_back(method);
}