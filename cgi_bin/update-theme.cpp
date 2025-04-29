// update-theme.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

std::string parse_post_data()
{
	std::string post_data;
	char c;

	while (std::cin.get(c))
		post_data += c;
	return (post_data);
}

std::string get_value(const std::string& data, const std::string& key)
{
	size_t pos = data.find(key + "=");
	if (pos == std::string::npos)
		return ("");
	pos += key.length() + 1;
	size_t end = data.find('&', pos);
	return (data.substr(pos, end - pos));
}

std::string get_session_path(const std::string& sessionID)
{
	return ("www/sessions_bin/session_" + sessionID + ".dat");
}

bool update_theme_in_session(const std::string& sessionID, const std::string& theme)
{
	std::string path = get_session_path(sessionID);
	std::ifstream in(path.c_str());
	if (!in.is_open())
		return (false);

	std::ostringstream updated;
	std::string line;
	bool themeUpdated = false;

	while (std::getline(in, line))
	{
		size_t eq = line.find('=');
		if (eq == std::string::npos)
		{
			updated << line << "\n";
			continue;
		}
		std::string key = line.substr(0, eq);
		if (key == "theme")
		{
			updated << "theme=" << theme << "\n";
			themeUpdated = true;
		}
		else
			updated << line << "\n";
	}
	in.close();

	if (!themeUpdated)
		updated << "theme=" << theme << "\n"; // Ajoute si jamais pas trouvé

	std::ofstream out(path.c_str());
	if (!out.is_open())
		return (false);
	out << updated.str();
	out.close();
	return (true);
}

int main()
{
	std::cout << "Content-Type: text/plain\r\n\r\n";

	const char* contentLength = getenv("CONTENT_LENGTH");
	if (!contentLength)
	{
		std::cout << "No POST data\n";
		return (1);
	}

	std::string post_data = parse_post_data();

	std::string sessionID = get_value(post_data, "sessionID");
	std::string theme = get_value(post_data, "theme");

	if (sessionID.empty() || theme.empty())
	{
		std::cout << "Missing sessionID or theme\n";
		return (1);
	}

	if (!update_theme_in_session(sessionID, theme))
	{
		std::cout << "Failed to update theme\n";
		return (1);
	}

	std::cout << "Theme updated successfully\n";
	return (0);
}
