#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../WebServ.hpp"

class Block {

    private:
        std::string _name;
        std::map<std::string, std::string> _directive;
        std::vector<Block> _childBlock;
       
    public:
        
        Block();
        ~Block();
        
        /* setters */

        void setName(std::string name);
        void setDirective(std::string key, std::string value);

        /* getters */

        std::string getName() const;
        std::map<std::string, std::string> getDirective() const;
        std::vector<Block> getChildBlock() const;

        /* adding a new block to vector */
        
        void addChildBlock(const Block& child);
};

#endif