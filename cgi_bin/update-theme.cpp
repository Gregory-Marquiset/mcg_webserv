// update-theme.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <map>

std::map<std::string, std::string> parsePostData(const std::string& data) {
	std::map<std::string, std::string> result;
	std::istringstream stream(data);
	std::string pair;

	while (std::getline(stream, pair, '&')) {
		size_t equal = pair.find('=');
		if (equal != std::string::npos) {
			std::string key = pair.substr(0, equal);
			std::string value = pair.substr(equal + 1);
			result[key] = value;
		}
	}
	return result;
}

void updateThemeInSessionFile(const std::string& sessionID, const std::string& theme) {
	std::string path = "sessions_bin/session_" + sessionID + ".dat";
	std::ifstream infile(path);
	if (!infile)
		return;

	std::ostringstream buffer;
	std::string line;
	while (std::getline(infile, line)) {
		if (line.find("theme=") == 0)
			buffer << "theme=" << theme << "\n";
		else
			buffer << line << "\n";
	}
	infile.close();

	std::ofstream outfile(path);
	outfile << buffer.str();
	outfile.close();
}

int main() {
	std::cout << "Content-Type: text/plain\n\n";

	char* lenStr = getenv("CONTENT_LENGTH");
	if (!lenStr) {
		std::cout << "No content.\n";
		return 0;
	}

	int len = std::atoi(lenStr);
	std::string data(len, 0);
	std::cin.read(&data[0], len);

	std::map<std::string, std::string> params = parsePostData(data);
	if (params.count("sessionID") && params.count("theme")) {
		updateThemeInSessionFile(params["sessionID"], params["theme"]);
		std::cout << "OK";
	} else {
		std::cout << "Missing sessionID or theme";
	}
	return 0;
}
