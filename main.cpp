#include "includes/server/ASocket.hpp"
#include "includes/server/ListeningSocket.hpp"
#include "includes/server/Server.hpp"
#include "includes/configFile/Block.hpp"
#include "includes/configFile/RecupBlockContent.hpp"
#include "includes/configFile/ReadyBlock.hpp"
// #include "includes/configFile/utils.hpp"
#include "includes/epollManager/EPollManager.hpp"

int main(int argc, char **argv) {

    if (argc == 2) {
        
        /* ================= PARSE & GET DATA FROM CONFIG FILE ======================== */

        RecupBlockContent rawConfig;

        std::string confFile = rawConfig.storeConfigFile(argv[1]);
        rawConfig.createServerBlocks(confFile);

        ReadyBlock data;

        data.setReadyBlock(rawConfig);

        /* ================================================================ */

        // Server server(AF_INET, SOCK_STREAM, 0, data.getListen(), INADDR_ANY, 3);
        
        // while (1) {
        //     std::cout << "======= WAITING =======" << std::endl;
        //     server.serverHandlingEvents(data.getRoot(), data.getIndex());
        //     std::cout << "======= EVENT HANDLING DONE =======" << std::endl;
        // }
    }
    else
        std::cerr << "Invalid Args: usage: ./webserv [configuration file]" << std::endl;
}
