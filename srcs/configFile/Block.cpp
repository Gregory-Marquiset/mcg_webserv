#include "../../includes/configFile/Block.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Block::Block() : _name(""), _directive(), _childBlock() {};

Block::~Block() {};

/* ================= SETTERS ======================== */

void Block::setName(std::string name) {
    this->_name = name;
}

void Block::setDirective(std::string key, std::string value) {
    this->_directive[key] = value; 
}

/* ================= GETTERS ======================== */

std::string Block::getName() const {
    return (this->_name);
}

std::map<std::string, std::string> Block::getDirective() const {
    return (this->_directive);
}

std::vector<Block> Block::getChildBlock() const {
    return (this->_childBlock);
}

/* add Child Block */

void Block::addChildBlock(const Block& child) {
    this->_childBlock.push_back(child);
}

