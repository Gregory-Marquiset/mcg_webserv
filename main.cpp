#include "includes/socketInit/ASocket.hpp"
#include "includes/socketInit/ListeningSocket.hpp"
#include "includes/serverHandling/Server.hpp"
#include "includes/configFile/Block.hpp"
#include "includes/configFile/Parse.hpp"
#include "includes/configFile/utils.hpp"

int main(int argc, char **argv) {

    if (argc == 2) {

        /* ================= WORK IN PROGRESS ======================== */

        std::cout << "Starting..." << std::endl;

        Parse config;

        std::string confFile = config.storeConfigFile(argv[1]);
        config.fillConfigBlockClass(confFile);

        int port;
        std::string root;
        std::string index;

        std::vector<Block> blocks = config.getConfigBlocks();

        std::vector<Block>::const_iterator it;
        for (it = blocks.begin(); it != blocks.end(); it++) {
          
            std::map<std::string, std::string> parentDirectives = it->getDirective();
            if (parentDirectives.find("listen") != parentDirectives.end()) {
                port = myStoi(parentDirectives["listen"]);
                std::cout << "port = " << port << std::endl;

                const std::vector<Block>& children = it->getChildBlock();            
                
                std::vector<Block>::const_iterator itChild;
                for (itChild = children.begin(); itChild != children.end(); itChild++) {
                    std::cout << "ICI" << std::endl;
                    std::map<std::string, std::string> childDirectives = itChild->getDirective();
                    printMap(itChild->getDirective());

                    if (childDirectives.find("root") != childDirectives.end()) {

                        root = childDirectives["root"];
                        std::cout << "root = " << root << std::endl;
                    }
                    
                    if (childDirectives.find("index") != childDirectives.end()) {
                        index = childDirectives["index"];
                        std::cout << "index = " << index << std::endl;
                    }
                }
            }
        }

        /* ================================================================ */

        Server server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 3);
        
        while (1) {
            std::cout << "======= WAITING =======" << std::endl;
            server.serverHandlingEvents(root, index);
            std::cout << "======= EVENT HANDLING DONE =======" << std::endl;
        }

    }
}
