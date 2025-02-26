#include "../../../includes/configFile/Parse.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */

Parse::Parse() {};

Parse::~Parse() {};

/* ================= GETTERS ======================== */

std::vector<Block>& Parse::getConfigBlocks() {
    return (this->_configBlocks);
}

/* ================= WORK IN PROGRESS ======================== */

std::string Parse::storeConfigFile(char *file) {

    std::string ret;
    std::string content;

    std::ifstream confFile(file);
    
    while (getline(confFile, content)) {
        ret += content + "\n";
    }
    confFile.close();
    return (ret);
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

void Parse::fillConfigBlockClass(std::string file) {
    
    std::string content;
    std::istringstream iss(file);

    std::string key;
    std::string value;

    Block parentBlock;
    Block childBlock;

    Block tmpFirstParent;
    tmpFirstParent.setName("testName");
    this->_configBlocks.push_back(tmpFirstParent);

    int parentFlag = 0; 
    int childFlag = 0;

    while (getline(iss, content)) {
        if (content.empty())
            continue ;

        for (size_t i = 0; i < content.size(); ++i) {
            
            if (content[i] == '{') {
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
            if (content[i] == '}') {

                if (parentFlag == 1 && childFlag == 0) {

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

            if (content[i] == '#') {
                std::string comment = content.substr(i);
                for (size_t i = 0; i < comment.size(); ++i) {
                    std::cout << comment[i] << std::endl;
                }
                std::cout << comment << std::endl;
            }

            if (content[i] == ';') {
                std::string tmpDirective = content.substr(0, i);
                std::string cleanDirective = trim(tmpDirective);

                key = "";
                value = "";

                int isKey = true;
                for (size_t j = 0; j < cleanDirective.size(); ++j) {
                    if (isspace(cleanDirective[j])) {
                        isKey = false ;
                        j++;
                    }
                    if (isKey)
                        key += cleanDirective[j];
                    else
                        value += cleanDirective[j];
                }

                if (childFlag == 0) {
                
                    if (this->_configBlocks.back().getName() == "testName") {
                        tmpFirstParent.setDirective(key, value);
                    }
                    else {
                        parentBlock.setDirective(key, value);
                    }
                }
                else {
                    childBlock.setDirective(key, value);
                }
            }
        }
    }
}

// get blocks and comments -> exctraction 
// get data -> clean to use 

