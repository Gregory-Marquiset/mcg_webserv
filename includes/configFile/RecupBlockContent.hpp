#ifndef RECUPBLOCKCONTENT_HPP
#define RECUPBLOCKCONTENT_HPP

#include "../WebServ.hpp"
#include "Block.hpp"

class RecupBlockContent {

    private:
        std::vector<Block> _serverBlocks;

    public:
        
        RecupBlockContent();
        ~RecupBlockContent();

        /* getter */

        std::vector<Block> getServerBlocks();

        /* fonctions pour Recup les blocks servers */
        
        std::string storeConfigFile(char *file);
        
        void createServerBlocks(std::string file);
        
        void handleEntranceBlock(std::string content, size_t i, int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock);
        void handleExitBlock(int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock);
        void handleDirectives(std::string content, size_t i, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block &childBlock);

};

std::string trim(const std::string& str);

#endif