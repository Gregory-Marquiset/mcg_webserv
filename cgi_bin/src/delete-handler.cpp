#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <map>

std::map<std::string, std::string>	parse_form_data()
{
	std::map<std::string, std::string>	result;
	std::string							input;
	char								c;

	while (std::cin.get(c))
		input += c;

	std::istringstream	ss(input);
	std::string			pair;
	while (std::getline(ss, pair, '&'))
	{
		size_t	eq = pair.find('=');
		if (eq != std::string::npos)
		{
			std::string	key = pair.substr(0, eq);
			std::string	value = pair.substr(eq + 1);
			result[key] = value;
		}
	}
	return (result);
}

int	main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	const char*	method = getenv("REQUEST_METHOD");
	if (!method || std::string(method) != "DELETE")
	{
		std::cout << "405 Method Not Allowed\n";
		return (1);
	}

	std::map<std::string, std::string>	form = parse_form_data();

	if (form.count("filename") == 0 || form["filename"].empty())
	{
		std::cout << "400 Bad Request: filename manquant\n";
		return (1);
	}

	std::string	filename = form["filename"];
	if (filename.find("..") != std::string::npos || filename.find('/') != std::string::npos)
	{
		std::cout << "400 Bad Request: nom de fichier invalide\n";
		return (1);
	}

	std::string	path = "./www/upload/" + filename;

	if (access(path.c_str(), F_OK) != 0)
	{
		std::cout << "404 Not Found: fichier inexistant\n";
		return (1);
	}

	if (remove(path.c_str()) != 0)
	{
		std::cout << "500 Internal Server Error: échec suppression\n";
		return (1);
	}

	std::cout << "200 OK: fichier supprimé avec succès\n";
	return (0);
}
