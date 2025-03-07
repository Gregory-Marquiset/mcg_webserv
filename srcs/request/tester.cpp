#include "../../includes/request/RequestParser.hpp"

int main ()
{
	std::string	GET_request = "GET /Afrique-cbeaaaaab.asp HTTP/1.1\r\n\
	Host: www.fourmishome.fr\r\n\
	User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:136.0) Gecko/20100101 Firefox/136.0\r\n\
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
	Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\r\n\
	Accept-Encoding: gzip, deflate, br, zstd\r\n\
	Connection: keep-alive\r\n\
	Cookie: cookieconsent_status=allow; uno=70e1159b3243f72553360df1a3eb2119\r\n\
	Upgrade-Insecure-Requests: 1\r\n\
	Sec-Fetch-Dest: document\r\n\
	Sec-Fetch-Mode: navigate\r\n\
	Sec-Fetch-Site: cross-site\r\n\
	Priority: u=0, i\r\n\
	\r\n";
	std::string	bad = "GET /Afrique-cbeaa%20ab.asp%20 HTTP/1.1\r\n";
	// std::string	POST_request;
	// std::string	DELETE_request;

	//std::cout << GET_request << std::endl << std::endl;

	RequestParser	req(bad);
}
