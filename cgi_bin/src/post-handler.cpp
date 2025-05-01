// post-handler.cgi
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

std::map<std::string, std::string>	parse_post()
{
	std::map<std::string, std::string>	data;
	std::string							post_data;
	char								c;

	while (std::cin.get(c))
		post_data += c;

	std::istringstream	ss(post_data);
	std::string			pair;

	while (std::getline(ss, pair, '&'))
	{
		size_t	eq = pair.find('=');
		if (eq != std::string::npos)
		{
			std::string	key = pair.substr(0, eq);
			std::string	value = pair.substr(eq + 1);
			data[key] = value;
		}
	}
	return (data);
}

int	main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	std::map<std::string, std::string>	data = parse_post();

	if (data.count("filename") == 0 || data["filename"].empty())
	{
		std::cout << "Missing filename field.";
		return (1);
	}

	std::string	filename = data["filename"];
	if (filename.find("..") != std::string::npos || filename.find('/') != std::string::npos)
	{
		std::cout << "Invalid filename.";
		return (1);
	}

	mkdir("www/upload", 0755);

	std::ofstream file(("www/upload/" + filename).c_str());
	if (!file)
	{
		std::cout << "Failed to create file.";
		return (1);
	}

	file << data["content"];
	file.close();

	std::cout << "File uploaded successfully.";
	return (0);
}
