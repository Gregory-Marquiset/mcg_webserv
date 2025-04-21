#include "includes/server/ASocket.hpp"
#include "includes/server/ListeningSocket.hpp"
#include "includes/server/Server.hpp"
#include "includes/configFile/Block.hpp"
#include "includes/configFile/RecupBlockContent.hpp"
#include "includes/configFile/LocationBlock.hpp"
#include "includes/configFile/ServerBlock.hpp"
#include "includes/configFile/CgiHandler.hpp"
#include "includes/configFile/HostHandler.hpp"
#include "includes/configFile/LocationBlock.hpp"
#include "includes/epollManager/EPollManager.hpp"

void printConfigFileData(std::vector<ServerBlock> serverBlocks) {

    if (serverBlocks.empty()) {
        std::cout << "No server in .conf" << std::endl;
    }

    for (size_t i = 0; i < serverBlocks.size(); ++i) {

        std::cout << "server { " << std::endl;
        for (size_t iHost = 0; iHost < serverBlocks[i].getHost().size(); ++iHost) {
            for (size_t j = 0; j < serverBlocks[i].getHost()[iHost].getHostName().size(); ++j) {
                std::cout << "      hostName: " << serverBlocks[i].getHost()[iHost].getHostName()[j] << std::endl;
            }
        }

        for (size_t iPort = 0; iPort < serverBlocks[i].getPort().size(); ++iPort) {
            std::cout << "      port: " << serverBlocks[i].getPort()[iPort] << std::endl;
        }
        std::cout << "      root: " << serverBlocks[i].getRoot() << std::endl;
        std::cout << "      index: " << serverBlocks[i].getIndex() << std::endl;
        std::cout << "      client max body size : " << serverBlocks[i].getClientMaxBodySize() << std::endl;
        std::cout << "      auto index: " << serverBlocks[i].getAutoIndex() << std::endl;
        
        if (serverBlocks[i].getRedirection().empty())
            std::cout << "      return: false (NULL)" << std::endl;
        else 
            std::cout << "      return: " << serverBlocks[i].getRedirection().front() << " " << serverBlocks[i].getRedirection()[1] <<std::endl;

        for (size_t iMethod = 0; iMethod < serverBlocks[i].getAllowMethods().size(); ++iMethod) {
            std::cout << "      method: " << serverBlocks[i].getAllowMethods()[iMethod] << std::endl;
        }

        std::vector<CgiHandler> cgiExtensions = serverBlocks[i].getCgiExtension();
        for (std::vector<CgiHandler>::iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
            std::cout << "      cgi_extension: " << "key: " << it->getKey() << " -> " << "value: " << it->getValue() << std::endl;
        }

        // location block
        for (size_t iLoc = 0; iLoc < serverBlocks[i].getLocationBlock().size(); ++iLoc) {

            std::cout << "       \n      location " << serverBlocks[i].getLocationBlock()[iLoc].getPath() << " { " << std::endl;

            std::cout << "          root: " << serverBlocks[i].getLocationBlock()[iLoc].getRoot() << std::endl;
            std::cout << "          index: " << serverBlocks[i].getLocationBlock()[iLoc].getIndex() << std::endl;
            std::cout << "          client max body size : " << serverBlocks[i].getLocationBlock()[iLoc].getClientMaxBodySize() << std::endl;
            std::cout << "          auto index: " << serverBlocks[i].getLocationBlock()[iLoc].getAutoIndex() << std::endl;

            if (serverBlocks[i].getLocationBlock()[iLoc].getRedirection().empty())
                std::cout << "          return: false (NULL)" << std::endl;
            else 
                std::cout << "          return: " << serverBlocks[i].getLocationBlock()[iLoc].getRedirection().front() << " " << serverBlocks[i].getLocationBlock()[iLoc].getRedirection()[1] <<std::endl;

            for (size_t iCgi = 0; iCgi < serverBlocks[i].getLocationBlock()[iLoc].getCgiExtension().size(); ++iCgi) {
                std::cout << "          cgi_extension: " << "key: " << serverBlocks[i].getLocationBlock()[iLoc].getCgiExtension()[iCgi].getKey() << " -> " << "value: " << serverBlocks[i].getLocationBlock()[iLoc].getCgiExtension()[iCgi].getValue() << std::endl;
            }

            for (size_t iMethod = 0; iMethod < serverBlocks[i].getLocationBlock()[iLoc].getAllowMethods().size(); ++iMethod) {
                std::cout << "          method: " << serverBlocks[i].getLocationBlock()[iLoc].getAllowMethods()[iMethod] << std::endl;
            }

            std::cout << "      }" << std::endl;
        }
        std::cout << "}" << std::endl;

        std::cout << "\n";
    }
}

volatile sig_atomic_t exitFlag = 0; 

void signal_handler(int s)
{
    (void)s;
    exitFlag = 1;
}

int main(int argc, char **argv) {

    if (argc == 2) {

        signal(SIGINT, signal_handler);

        /* ================= Analyse le fichier et extrait les blocs ======================== */

        RecupBlockContent rawConfig;

        std::string confFile = rawConfig.storeConfigFile(argv[1]);
        try {
            rawConfig.createTree(confFile);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }

        /* ================= Transforme les blocs en objets exploitables ======================== */

        ServerBlock data;
        std::vector<ServerBlock> serverBlocks;
        
        try {
            serverBlocks = data.createAllServerBlocks(rawConfig);
            printConfigFileData(serverBlocks);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }

        /* ================= Transforme les blocs en servers ======================== */

        std::vector<Server> servers;

        for (size_t i = 0; i < serverBlocks.size(); ++i) {

            servers.push_back(Server(serverBlocks[i]));
            servers[i].printServerInfo();
        }

        /* ================= Les servers sont sous surveillance ======================== */

        try {

            EPollManager epollManager(servers);

            while (exitFlag == 0) {
                epollManager.run();
                // std::cout << "===== Default Servers according to Port =====" << std::endl;
                // // std::cout << "server nb = " << servers.size() << std::endl;
                // for (size_t i = 0; i < servers.size(); ++i) {
                
                //     // std::cout << "size = " << servers[i].getServerStatusAccordingToPort().size() << std::endl;
                //     std::map<int, int> status = servers[i].getServerStatusAccordingToPort();
                //     for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
                //         std::cout << "Server " << i << " Is default server for port: " << it->first << " = " << it->second << std::endl;
                //     }
                // }
                // std::cout << "=============================================" << std::endl;
            }
            epollManager.clean();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return (1);
        }
    }
    else {
        std::cerr << "Invalid Args: usage: ./webserv [configuration file]" << std::endl;
        return (1);
    }
    return (0);
}
