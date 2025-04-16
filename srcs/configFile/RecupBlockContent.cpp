#include "../../includes/configFile/RecupBlockContent.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

RecupBlockContent::RecupBlockContent() {};

RecupBlockContent::~RecupBlockContent() {};

/* ================= GETTERS ======================== */

std::vector<Block> RecupBlockContent::getServerBlocks() {
    return (this->_serverBlocks);
}

/* ================= RECUP / HIERARCHISE BLOCKS ======================== */

/* stock le fichier .conf */

std::string RecupBlockContent::storeConfigFile(char *file) {

    std::string ret;
    std::string content;

    std::ifstream confFile(file);
    
    while (getline(confFile, content)) {
        ret += content + "\n";
    }
    confFile.close();
    return (ret);
}

std::string trim(const std::string& str) {

    size_t start = 0;
    while (start < str.size() && isspace(str[start])) {
        ++start;
    }

    size_t end = str.size();
    while (end > start && isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}

/* fonction principale pour hierarchiser les blocks (arbre parents - enfants) */

void RecupBlockContent::createTree(std::string file) {
     
    std::string content;
    std::istringstream iss(file);
    
    Block parentBlock;
    Block childBlock;
    
    int parentFlag = 0; 
    int childFlag = 0;

    Block tmpFirstParent;
    tmpFirstParent.setName("tmpBlock");
    this->_serverBlocks.push_back(tmpFirstParent);
    
    while (getline(iss, content)) {
        if (content.empty())
            continue ;

        for (size_t i = 0; i < content.size(); ++i) {
            
            // ignorer les commentaires 
            if (content[i] == '#') {
                std::cout << "coucou\n";
                break ;
            }

            // entree d un block 
            if (content[i] == '{') {
               handleEntranceBlock(content, i, parentFlag, childFlag, tmpFirstParent, parentBlock, childBlock);
            }

            // sortie d un block
            if (content[i] == '}') 
                handleExitBlock(parentFlag, childFlag, tmpFirstParent, parentBlock, childBlock);

            // init directives
            if (content[i] == ';') {
                handleDirectives(content, i, childFlag, tmpFirstParent, parentBlock, childBlock);
            }
        }
    }
}

/* gestion de l entree d un block */

void RecupBlockContent::handleEntranceBlock(std::string content, size_t i, int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock) {

    std::string tmpName = content.substr(0, i);
    std::string cleanName = trim(tmpName);

    if (parentFlag == 0) {
            
        tmpFirstParent.setName(cleanName);
        parentBlock.setName(cleanName);
        parentFlag += 1;
    }
    else  {
        childBlock.setName(cleanName);
        childFlag += 1;
    }
}

/* gestion de la sortie d un block */

void RecupBlockContent::handleExitBlock(int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock) {

    if (parentFlag == 1 && childFlag == 0) { // la je suis dans un parent 

        if (this->_serverBlocks.back().getName() == "tmpBlock") {
            parentBlock = tmpFirstParent;
            this->_serverBlocks.erase(this->_serverBlocks.begin());
        }
        this->_serverBlocks.push_back(parentBlock);
        parentBlock = Block();
        parentFlag -= 1;
    }
    else { // la je suis dans un enfant 
        
        if (this->_serverBlocks.back().getName() == "tmpBlock") {
            tmpFirstParent.addChildBlock(childBlock);
        }
        else {
            parentBlock.addChildBlock(childBlock);
        }
        childBlock = Block();
        childFlag -= 1;
    }
}

/* initialisation des directives */

void RecupBlockContent::handleDirectives(std::string content, size_t i, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block &childBlock) {

    std::string key = "";
    std::string value = "";
    int isKey = 1;
    std::string tmpDirective = content.substr(0, i);
    std::string cleanDirective = trim(tmpDirective);
    // std::cout << "---> " << cleanDirective << std::endl;

    for (size_t j = 0; j < cleanDirective.size(); ++j) {
        if (isspace(cleanDirective[j]) && isKey == 1) {
            isKey = 0 ;
            j++;
        }
        if (isKey)
            key += cleanDirective[j];
        else 
            value += cleanDirective[j];
    }
   
    if (childFlag == 0) {
    
        if (this->_serverBlocks.back().getName() == "tmpBlock")
            tmpFirstParent.setDirective(key, value);
        else 
            parentBlock.setDirective(key, value);
    }
    else 
        childBlock.setDirective(key, value);
}

// un bloc c est forcement un nom + {}
// des directives c est forcement une cle et une valeur 

// read file line by line 
// -> look for {
// then look for }

// if another { alors block enfant 

// store everything that is in between
// get what is on same line that { -> should be name
// then get directives 

/* !!!! si l ordre des directives n apparait pas comme sur le .conf c est normal, j utilise une map qui range par ordre alphabetique */