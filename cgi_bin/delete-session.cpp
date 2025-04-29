#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <string>

std::string	parse_session_id()
{
	std::string	post_data;
	char		c;

	while (std::cin.get(c))
		post_data += c;

	size_t pos = post_data.find("sessionID=");
	if (pos == std::string::npos)
		return ("");

	std::string id = post_data.substr(pos + 10);
	size_t end = id.find('&');
	if (end != std::string::npos)
		id = id.substr(0, end);

	return (id);
}

std::string get_session_path(const std::string& sessionID)
{
	return ("www/sessions_bin/session_" + sessionID + ".dat");
}

int main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	std::string sessionID = parse_session_id();
	if (sessionID.empty())
	{
		std::cout << "Session ID manquant\n";
		return (1);
	}

	std::string path = get_session_path(sessionID);
	if (unlink(path.c_str()) == 0)
		std::cout << "Session supprimée\n";
	else
	{
		std::cout << "Erreur lors de la suppression\n";
		return (1);
	}
	return (0);
}
