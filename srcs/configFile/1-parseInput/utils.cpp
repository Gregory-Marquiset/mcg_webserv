#include "../../../includes/WebServ.hpp"

std::string trim(const std::string& str) {

    size_t start = 0;
    while (start < str.size() && isspace(str[start])) {
        ++start;
    }

    size_t end = str.size();
    while (end > start && isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}

void printMap(const std::map<std::string, std::string>& directive) {
    std::map<std::string, std::string>::const_iterator it;
    int i = 0 ;
    for (it = directive.begin(); it != directive.end(); ++it) {
        std::cout << "directive: " << i << " key: " << it->first << " | " << "value: " << it->second << std::endl;
        i++;
    }
}

int myStoi(std::string& s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}