#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>

std::string get_cookie_value(const std::string& cookies, const std::string& key)
{
	std::istringstream ss(cookies);
	std::string token;
	while (std::getline(ss, token, ';')) {
		size_t eq = token.find('=');
		if (eq != std::string::npos) {
			std::string k = token.substr(0, eq);
			std::string v = token.substr(eq + 1);
			// Trim
			k.erase(0, k.find_first_not_of(" \t\r\n"));
			k.erase(k.find_last_not_of(" \t\r\n") + 1);
			v.erase(0, v.find_first_not_of(" \t\r\n"));
			v.erase(v.find_last_not_of(" \t\r\n") + 1);
			if (k == key) return v;
		}
	}
	return "";
}


bool file_exists(const std::string& filename)
{
	return (access(filename.c_str(), F_OK) != -1);
}

std::string generate_session_id()
{
	std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
	std::stringstream ss;
	unsigned char byte;
	for (int i = 0; i < 16; ++i) {
		urandom.read(reinterpret_cast<char*>(&byte), sizeof(byte));
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
	}
	return ss.str();
}

std::string get_session_path(const std::string& sessionID)
{
	return "sessions_bin/session_" + sessionID + ".dat";
}

std::string create_session_file(const std::string& sessionID)
{
	std::ofstream f(get_session_path(sessionID).c_str());
	f << "sessionid=" << sessionID << "\nusername=guest\nstatus=anonymous\nemail=none\npassword=none\ntheme=dark-theme";
	f.close();
	return sessionID;
}

std::string read_session(const std::string& sessionID)
{
	std::ifstream f(get_session_path(sessionID).c_str());
	std::string line, output;
	while (std::getline(f, line)) {
		size_t eq = line.find('=');
		if (eq != std::string::npos) {
			std::string key = line.substr(0, eq);
			std::string value = line.substr(eq + 1);
			output += "\"" + key + "\": \"" + value + "\", ";
		}
	}
	if (!output.empty()) output.pop_back(), output.pop_back();
	return "{ " + output + " }";
}

int main()
{
	std::string cookies = getenv("HTTP_COOKIE") ? getenv("HTTP_COOKIE") : "";
	std::string sessionID = get_cookie_value(cookies, "sessionID");
	std::string responseHeader = "Content-Type: application/json\r\n";

	std::cerr << "[DEBUG] HTTP_COOKIE=" << cookies << std::endl;
	std::cerr << "[DEBUG] sessionID=" << sessionID << std::endl;
	
	bool isNewSession = false;

	if (sessionID.empty() || !file_exists(get_session_path(sessionID))) {
		sessionID = generate_session_id();
		create_session_file(sessionID);
		isNewSession = true;
	}

	if (isNewSession)
		responseHeader += "Set-Cookie: sessionID=" + sessionID + "; Path=/\r\n";

	std::string jsonData = read_session(sessionID);
	std::cout << responseHeader << "\r\n" << jsonData;

	return 0;
}
