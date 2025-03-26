#include "../../includes/configFile/CgiHandler.hpp" 

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

CgiHandler::CgiHandler() {};

CgiHandler::~CgiHandler() {};

/* ================= SETTERS ======================== */

void CgiHandler::setKey(std::string key) {
    this->_key = key;
}

void CgiHandler::setValue(std::string value) {
    this->_value = value;
}

std::string CgiHandler::getKey() const {
    return (this->_key);
}

std::string CgiHandler::getValue() const {
    return (this->_value);
}

void CgiHandler::parseCgi(std::string cgiLine) {

    size_t pos = cgiLine.find(' ');

    if (pos != std::string::npos) {
        std::string key = cgiLine.substr(0, pos);
        std::string value = cgiLine.substr(pos + 1);
        
        this->setKey(key);
        this->setValue(value);
    }
}