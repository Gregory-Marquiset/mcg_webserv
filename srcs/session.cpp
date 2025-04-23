#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <map>

std::string	we_getSessionIDFromCookieHeader(const std::string &cookieHeader)
{
    std::istringstream ss(cookieHeader);
    std::string pair;

    while (std::getline(ss, pair, ';'))
    {
        size_t start = pair.find_first_not_of(" ");
        if (start == std::string::npos)
            continue;
        pair = pair.substr(start);

        size_t pos = pair.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = pair.substr(0, pos);
        std::string value = pair.substr(pos + 1);

        if (key == "sessionID")
            return value;
    }
    return "";
}

bool we_ensureSessionDirectoryExists()
{
    if (access("sessions_bin", F_OK) == -1)
    {
        if (mkdir("sessions_bin", 0755) == -1)
        {
            std::cerr << "Erreur : Impossible de créer le dossier 'sessions_bin'\n";
            return false;
        }
    }
    return true;
}

bool we_saveSessionToFile(const std::string &sessionID, const std::string &data)
{
    if (!we_ensureSessionDirectoryExists())
        return false;

    std::string filename = "sessions_bin/session_" + sessionID + ".dat";
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);

    if (!file)
    {
        std::cerr << "Erreur : Impossible de créer le fichier de session " << filename << "\n";
        return false;
    }
    file << data;
    file.close();
    return true;
}

std::string we_generateSessionID()
{
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);

    if (!urandom)
    {
        std::cerr << "Erreur : Impossible d'accéder à /dev/urandom\n";
        return "500 Internal Server Error";
    }

    std::stringstream ss;
    unsigned char randomByte;

    for (int i = 0; i < 16; i++)
    {
        urandom.read(reinterpret_cast<char *>(&randomByte), sizeof(randomByte));
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)randomByte;
    }
    urandom.close();

    return ss.str();
}

std::string we_createSession()
{
    std::string sessionID = we_generateSessionID();
    std::string initialData = "user=guest\npassword=none\nstatus=anonymous\ntheme=1";

    if (sessionID == "500 Internal Server Error" || !we_saveSessionToFile(sessionID, initialData))
        return "500 Internal Server Error";

    return sessionID;
}

std::string we_loadSessionFromFile(const std::string &sessionID)
{
    std::string filename = "sessions_bin/session_" + sessionID + ".dat";
    std::ifstream file(filename.c_str());

    if (!file)
    {
        std::cerr << "Erreur : Impossible de lire le fichier de session " << filename << "\n";
        return "500 Internal Server Error";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

// =============================== TEST MAIN ================================

int main(int argc, char **argv)
{
    std::string cookieHeader;

    if (argc > 1)
        cookieHeader = argv[1];

    std::string sessionID = we_getSessionIDFromCookieHeader(cookieHeader);

    if (sessionID.empty() || we_loadSessionFromFile(sessionID) == "500 Internal Server Error")
    {
        sessionID = we_createSession();
        std::cout << "Set-Cookie: sessionID=" << sessionID << "\n";
    }
    else
    {
        std::cout << "Session récupérée : " << sessionID << "\n";
    }

    std::cout << "Contenu session :\n" << we_loadSessionFromFile(sessionID) << std::endl;

    return 0;
}
