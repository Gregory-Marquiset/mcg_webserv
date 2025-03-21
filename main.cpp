#include "includes/server/ASocket.hpp"
#include "includes/server/ListeningSocket.hpp"
#include "includes/server/Server.hpp"
#include "includes/configFile/Block.hpp"
#include "includes/configFile/RecupBlockContent.hpp"
#include "includes/configFile/LocationBlock.hpp"
#include "includes/configFile/ServerBlock.hpp"
#include "includes/configFile/LocationBlock.hpp"
#include "includes/epollManager/EPollManager.hpp"

int main(int argc, char **argv) {

    if (argc == 2) {

        /* ================= Analyse le fichier et extrait les blocs ======================== */

        RecupBlockContent rawConfig;

        std::string confFile = rawConfig.storeConfigFile(argv[1]);
        rawConfig.createTree(confFile);

        /* ================= Transforme les blocs en objets exploitables ======================== */

        ServerBlock data;
        std::vector<ServerBlock> serverBlocks = data.getAllServerBlocks(rawConfig);

        /* ================= Transforme les blocs en servers ======================== */

        std::vector<Server> servers;

        for (size_t i = 0; i < serverBlocks.size(); ++i) {
            servers.push_back(Server(serverBlocks[i]));
            servers[i].printServerInfo();
        }
        /* ================= Les servers sont sous surveillance ======================== */

        EPollManager epollManager(servers);
        // epollManager.run();
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
