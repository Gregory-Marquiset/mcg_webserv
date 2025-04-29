// signup.cpp → compilé en signup.cgi
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

std::map<std::string, std::string>	parse_post_data()
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
		size_t eq = pair.find('=');
		if (eq != std::string::npos)
		{
			std::string	key = pair.substr(0, eq);
			std::string	value = pair.substr(eq + 1);
			for (size_t i = 0; i < value.length(); ++i)
			{
				if (value[i] == '+')
					value[i] = ' ';
			}
			data[key] = value;
		}
	}
	return (data);
}

std::string get_session_path(const std::string& sessionID)
{
	return ("www/sessions_bin/session_" + sessionID + ".dat");
}

void update_session(const std::string& path, const std::map<std::string, std::string>& data)
{
	std::ostringstream	updated;
	std::ifstream		in(path);
	std::string			line;

	while (std::getline(in, line))
	{
		size_t	eq = line.find('=');
		if (eq == std::string::npos)
			continue;

		std::string	key = line.substr(0, eq);
		if (data.count(key))
			updated << key << "=" << data.at(key) << "\n";
		else
			updated << line << "\n";
	}
	in.close();
	std::ofstream	out(path);
	out << updated.str();
	out.close();
}

int	main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	const char* len_str = getenv("CONTENT_LENGTH");
	if (!len_str)
	{
		std::cout << "Aucune donnée reçue\n";
		return (1);
	}

	std::map<std::string, std::string> data = parse_post_data();

	if (data["sessionID"].empty())
	{
		std::cout << "Session invalide\n";
		return (1);
	}
	if (data["username"] == "mlanglois" || data["username"] == "cdutel" || data["username"] == "gmarquis")
		data["status"] = "admin";
	else
		data["status"] = "user";
	std::string nData = data["email"];
	std::size_t	pos = nData.find("%40");
	nData.replace(pos, 3, "@");
	data["email"] = nData;
	std::string path = get_session_path(data["sessionID"]);
	update_session(path, data);

	std::cout << "OK\n";
	return (0);
}
