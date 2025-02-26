#include "../../../includes/configFile/Recup.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Recup::Recup() {};

Recup::~Recup() {};

/* ================= GETTERS ======================== */

std::vector<Block>& Recup::getConfigBlocks() {
    return (this->_configBlocks);
}

/* ================= RECUP DATA ======================== */

std::string Recup::storeConfigFile(char *file) {

    std::string ret;
    std::string content;

    std::ifstream confFile(file);
    
    while (getline(confFile, content)) {
        ret += content + "\n";
    }
    confFile.close();
    return (ret);
}

/* fonction principale pour recuperer les blocks */

void Recup::recupBlocks(std::string file) {
     
    std::string content;
    std::istringstream iss(file);
    
    Block parentBlock;
    Block childBlock;
    
    int parentFlag = 0; 
    int childFlag = 0;

    Block tmpFirstParent;
    tmpFirstParent.setName("testName");
    this->_configBlocks.push_back(tmpFirstParent);
    
    while (getline(iss, content)) {
        if (content.empty())
            continue ;

        for (size_t i = 0; i < content.size(); ++i) {
            
            // entree d un block 
            if (content[i] == '{') {
               handleEntranceBlock(content, i, parentFlag, childFlag, tmpFirstParent, parentBlock, childBlock);
            }

            // sortie d un block
            if (content[i] == '}') 
                handleExitBlock(parentFlag, childFlag, tmpFirstParent, parentBlock, childBlock);

            // ignorer les commentaires 
            if (content[i] == '#')
                continue ;

            // init directives
            if (content[i] == ';') {
                handleDirectives(content, i, childFlag, tmpFirstParent, parentBlock, childBlock);
            }
        }
    }
}

/* gestion de l entree */

void Recup::handleEntranceBlock(std::string content, size_t i, int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock) {

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

/* gestion de la sortie */

void Recup::handleExitBlock(int &parentFlag, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block& childBlock) {

    if (parentFlag == 1 && childFlag == 0) { // la je suis dans un parent 

        if (this->_configBlocks.back().getName() == "testName") {
            parentBlock = tmpFirstParent;
            this->_configBlocks.erase(this->_configBlocks.begin());
        }
        this->_configBlocks.push_back(parentBlock);
        parentBlock = Block();
        parentFlag -= 1;
    }
    else { // la je suis dans un enfant 
        
        if (this->_configBlocks.back().getName() == "testName") {
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

void Recup::handleDirectives(std::string content, size_t i, int &childFlag, Block &tmpFirstParent, Block &parentBlock, Block &childBlock) {

    std::string key = "";
    std::string value = "";
    int isKey = 1;
    std::string tmpDirective = content.substr(0, i);
    std::string cleanDirective = trim(tmpDirective);

    for (size_t j = 0; j < cleanDirective.size(); ++j) {
        if (isspace(cleanDirective[j])) {
            isKey = 0 ;
            j++;
        }
        if (isKey)
            key += cleanDirective[j];
        else
            value += cleanDirective[j];
    }
   
    if (childFlag == 0) {
    
        if (this->_configBlocks.back().getName() == "testName")
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