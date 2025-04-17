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

// ca a l air a peu pres ok pour l attributaion des servers par defaut
// gerer les invalides files : non exiting
// gerer que les requetes visent les bons servers et dans le cas echeant le server par defaut
// leak lorsque je exit failure mais sinon c est good 

void printConfigFileData(std::vector<ServerBlock> serverBlocks) {

    if (serverBlocks.empty()) {
        std::cout << "No server in .conf" << std::endl;
    }

    for (size_t i = 0; i < serverBlocks.size(); ++i) {

        std::cout << "server { " << std::endl;
        for (size_t iHost = 0; iHost < serverBlocks[i].getHost().size(); ++iHost) {
            std::cout << "      hostName: " << serverBlocks[i].getHost()[iHost].getHostName() << std::endl;
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

int main(int argc, char **argv) {

    if (argc == 2) {

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
            // servers[i].printServerInfo();
        }

        // /* ================= Les servers sont sous surveillance ======================== */

        EPollManager epollManager(servers);

        std::cout << "===== Default Servers according to Port =====" << std::endl;
        // std::cout << "server nb = " << servers.size() << std::endl;
        for (size_t i = 0; i < servers.size(); ++i) {

            // std::cout << "size = " << servers[i].getServerStatusAccordingToPort().size() << std::endl;
            std::map<int, int> status = servers[i].getServerStatusAccordingToPort();
            for (std::map<int, int>::iterator it = status.begin(); it != status.end(); ++it) {
                std::cout << "Server " << i << " Is default server for port: " << it->first << " = " << it->second << std::endl;
            }
        }
        std::cout << "=============================================" << std::endl;

        epollManager.run();
    }
    else
        std::cerr << "Invalid Args: usage: ./webserv [configuration file]" << std::endl;
}

// Les classes:

// Block                Structure d'un block de config
// RecupBlockContent	Analyse le fichier et extrait les blocs
// ServerBlock	        Transforme les blocs en objets exploitables
// Server	            Représente un serveur unique basé sur un ServerBlock + Ajout d'une ListeningSocket
// ServerManager	    Gère plusieurs serveurs et stocke un std::vector<Server>

// problem avec le hostname
// Launch multiple servers at the same time with different configurations but with common ports. Does it work? If it does, ask why the server should work if one of the configurations isn't functional. Keep going.
// donc try catch blocks
// possible leaks