#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

std::map<std::string, std::string>	we_parse_post_data()
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

			for (size_t i = 0; i < value.length(); ++i)
			{
				if (value[i] == '+')
					value[i] = ' ';
			}

			size_t	pos;
			while ((pos = value.find("%40")) != std::string::npos)
				value.replace(pos, 3, "@");
			data[key] = value;
		}
	}
	return (data);
}

std::string	we_get_session_path(const std::string& sessionID)
{
	return ("www/sessions_bin/session_" + sessionID + ".dat");
}

void	we_update_session(const std::string& path, const std::map<std::string, std::string>& data)
{
	std::ostringstream	updated;
	std::ifstream		in(path.c_str());
	std::string			line;

	while (std::getline(in, line))
	{
		size_t	eq = line.find('=');
		if (eq == std::string::npos)
			continue;

		std::string											key = line.substr(0, eq);
		std::map<std::string, std::string>::const_iterator	it = data.find(key);
		if (it != data.end())
			updated << key << "=" << it->second << "\n";
		else
			updated << line << "\n";
	}
	in.close();

	std::ofstream	out(path.c_str());
	out << updated.str();
	out.close();
}

int	main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	const char*	len_str = getenv("CONTENT_LENGTH");
	if (!len_str)
	{
		std::cout << "Aucune donnée reçue\n";
		return 1;
	}

	std::map<std::string, std::string>	data = we_parse_post_data();

	if (data.find("sessionID") == data.end() || data["sessionID"].empty())
	{
		std::cout << "Session invalide\n";
		return 1;
	}

	if (data["username"] == "mlanglois" || data["username"] == "cdutel" || data["username"] == "gmarquis")
		data["status"] = "admin";
	else
		data["status"] = "user";

	if (data.find("email") != data.end())
	{
		std::string&	email = data["email"];
		size_t			pos;
		while ((pos = email.find("%40")) != std::string::npos)
			email.replace(pos, 3, "@");
	}

	std::string	path = we_get_session_path(data["sessionID"]);
	we_update_session(path, data);

	std::cout << "OK\n";
	return 0;
}
