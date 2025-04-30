#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

std::string	we_get_cookie_value(const std::string& cookies, const std::string& key)
{
	std::istringstream	ss(cookies);
	std::string			token;

	while (std::getline(ss, token, ';'))
	{
		size_t	eq = token.find('=');
		if (eq != std::string::npos)
		{
			std::string	k = token.substr(0, eq);
			std::string	v = token.substr(eq + 1);

			k.erase(0, k.find_first_not_of(" \t\r\n"));
			k.erase(k.find_last_not_of(" \t\r\n") + 1);
			v.erase(0, v.find_first_not_of(" \t\r\n"));
			v.erase(v.find_last_not_of(" \t\r\n") + 1);

			if (k == key)
				return (v);
		}
	}
	return ("");
}

bool	we_file_exists(const std::string& filename)
{
	return (access(filename.c_str(), F_OK) != -1);
}

std::string	we_generate_session_id()
{
	std::string	hex_chars = "0123456789abcdef";
	std::string	id;
	std::srand(std::time(NULL) ^ getpid());

	for (int i = 0; i < 32; ++i)
		id += hex_chars[std::rand() % 16];

	return (id);
}

std::string	we_get_session_path(const std::string& sessionID)
{
	return ("www/sessions_bin/session_" + sessionID + ".dat");
}

std::string	we_create_session_file(const std::string& sessionID)
{
	std::ofstream	f(we_get_session_path(sessionID).c_str());
	f << "sessionid=" << sessionID << "\n"
	  << "username=Anonymous\n"
	  << "status=guest\n"
	  << "email=none\n"
	  << "password=none\n"
	  << "theme=light-theme";
	f.close();
	return (sessionID);
}

std::string	we_read_session(const std::string& sessionID)
{
	std::ifstream	f(we_get_session_path(sessionID).c_str());
	std::string		line, output;

	while (std::getline(f, line))
	{
		size_t	eq = line.find('=');
		if (eq != std::string::npos)
		{
			std::string	key = line.substr(0, eq);
			std::string	value = line.substr(eq + 1);
			output += "\"" + key + "\": \"" + value + "\", ";
		}
	}
	if (!output.empty())
	{
		output.erase(output.length() - 2);
	}
	return ("{ " + output + " }");
}

int	main()
{
	std::string	cookies = getenv("HTTP_COOKIE") ? getenv("HTTP_COOKIE") : "";
	std::string	sessionID = we_get_cookie_value(cookies, "sessionID");
	std::string	responseHeader = "Content-Type: application/json\r\n";

	bool	isNewSession = false;

	if (sessionID.empty() || !we_file_exists(we_get_session_path(sessionID)))
	{
		sessionID = we_generate_session_id();
		we_create_session_file(sessionID);
		isNewSession = true;
	}

	if (isNewSession)
		responseHeader += "Set-Cookie: sessionID=" + sessionID + "; Path=/\r\n";

	std::string	jsonData = we_read_session(sessionID);
	std::cout << responseHeader << "\r\n" << jsonData;

	return (0);
}
