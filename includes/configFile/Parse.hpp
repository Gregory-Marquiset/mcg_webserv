#ifndef PARSE_HPP
#define PARSE_HPP

#include "../WebServ.hpp"
#include "Block.hpp"

class Parse {

    private:
        std::vector<Block> _configBlocks;

    public:
        
        Parse();
        ~Parse();

        std::vector<Block>& getConfigBlocks();

        /* fonctions pour parse */
        
        std::string storeConfigFile(char *file);
        void fillConfigBlockClass(std::string file);

};

std::string trim(const std::string& str);

#endif