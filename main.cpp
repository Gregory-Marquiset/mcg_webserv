#include "includes/socketInit/ASocket.hpp"
#include "includes/socketInit/ListeningSocket.hpp"
#include "includes/serverHandling/Server.hpp"
#include "includes/configFile/Block.hpp"
#include "includes/configFile/Parse.hpp"
#include "includes/configFile/utils.hpp"
#include "includes/assignValues/ConfigData.hpp"

int main(int argc, char **argv) {

    if (argc == 2) {
        
        /* ================= PARSE & GET DATA FROM CONFIG FILE ======================== */

        Parse rawConfig;

        std::string confFile = rawConfig.storeConfigFile(argv[1]);
        rawConfig.fillConfigBlockClass(confFile);

        ConfigData data;

        data.setConfigData(rawConfig);

        /* ================================================================ */

        Server server(AF_INET, SOCK_STREAM, 0, data.getListen(), INADDR_ANY, 3);
        
        while (1) {
            std::cout << "======= WAITING =======" << std::endl;
            server.serverHandlingEvents(data.getRoot(), data.getIndex());
            std::cout << "======= EVENT HANDLING DONE =======" << std::endl;
        }
    }
    else
        std::cerr << "Invalid Args: usage: ./webserv file.conf" << std::endl;
}
