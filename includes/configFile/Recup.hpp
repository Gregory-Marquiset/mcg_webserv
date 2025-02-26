#ifndef RECUP_HPP
#define RECUP_HPP

#include "../WebServ.hpp"
#include "Block.hpp"

class Recup {

    private:
        std::vector<Block> _configBlocks;

    public:
        
        Recup();
        ~Recup();

        std::vector<Block>& getConfigBlocks();

        /* fonctions pour Recup */
        
        std::string storeConfigFile(char *file);
        void recupBlocks(std::string file);
        
        void handleEntranceBlock(std::string content, size_t i, int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock);
        void handleExitBlock(int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock);
        void handleDirectives(std::string content, size_t i, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block &childBlock);

};

std::string trim(const std::string& str);

#endif