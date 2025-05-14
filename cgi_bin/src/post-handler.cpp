// post-handler.cgi
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <cctype>

int	hex_char_to_int(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (-1);
}

std::string	url_decode(const std::string &str)
{
	std::string	decoded;

	for (std::size_t i = 0; i < str.length(); ++i)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			int	high = hex_char_to_int(str[i + 1]);
			int	low  = hex_char_to_int(str[i + 2]);
			if (high != -1 && low != -1)
			{
				char	c = static_cast<char>((high << 4) | low);
				decoded += c;
				i += 2;
			}
		}
		else if (str[i] == '+')
		{
			decoded += ' ';
		}
		else
		{
			decoded += str[i];
		}
	}
	return (decoded);
}

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
			std::string	key = url_decode(pair.substr(0, eq));
			std::string	value = url_decode(pair.substr(eq + 1));
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

	std::string	fullpath = "www/upload/" + filename;

	int	fd = open(fullpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
	if (fd < 0)
	{
		std::cout << "Failed to create file " << filename << std::endl;
		return (1);
	}

	std::string content = data["content"];
	if (write(fd, content.c_str(), content.size()) < 0)
	{
		std::cout << "Failed to write in file " << fullpath << std::endl;
		close(fd);
		return (1);
	}

	close(fd);

	std::cout	<< "File uploaded successfully.";
	return (0);
}
