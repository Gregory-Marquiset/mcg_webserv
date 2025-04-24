// update-theme.cpp
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdlib>

std::string we_getSessionIDFromCookieHeader(const std::string &cookieHeader)
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
        return mkdir("sessions_bin", 0755) != -1;
    return true;
}

std::string we_generateSessionID()
{
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    if (!urandom)
        return "";

    std::stringstream ss;
    unsigned char byte;
    for (int i = 0; i < 16; i++)
    {
        urandom.read(reinterpret_cast<char *>(&byte), sizeof(byte));
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return ss.str();
}

bool we_sessionExists(const std::string &id)
{
    std::string filename = "sessions_bin/session_" + id + ".dat";
    return access(filename.c_str(), F_OK) != -1;
}

bool we_createSessionFile(const std::string &id)
{
    std::string filename = "sessions_bin/session_" + id + ".dat";
    std::ofstream file(filename.c_str());
    if (!file)
        return false;
    file << "theme=dark-theme\n"; // valeur par défaut
    return true;
}

int main()
{
    std::string cookieHeader;
    char *rawCookie = getenv("HTTP_COOKIE");
    if (rawCookie)
        cookieHeader = rawCookie;

    std::string sessionID = we_getSessionIDFromCookieHeader(cookieHeader);

    if (!we_ensureSessionDirectoryExists())
    {
        std::cout << "Status: 500 Internal Server Error\n\nErreur serveur.";
        return 1;
    }

    bool newSession = false;

    if (sessionID.empty() || !we_sessionExists(sessionID))
    {
        sessionID = we_generateSessionID();
        if (sessionID.empty() || !we_createSessionFile(sessionID))
        {
            std::cout << "Status: 500 Internal Server Error\n\nImpossible de créer une session.";
            return 1;
        }
        newSession = true;
    }

    std::cout << "Status: 200 OK\r\n";
    if (newSession)
        std::cout << "Set-Cookie: sessionID=" << sessionID << "; Path=/; HttpOnly\r\n";
    std::cout << "Content-Type: text/plain\r\n\r\n";
    std::cout << "Session active: " << sessionID << "\n";
    return 0;
}
