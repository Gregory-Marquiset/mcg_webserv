#include "../../includes/request/RequestParser.hpp"

int main ()
{
	std::string	GET_request = "GET /?app=desktop&hl=FR HTTP/3\r\n\
Host: www.youtube.com\r\n\
Content-Length: 6526\r\n\
Transfer-Encoding: chunked\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:136.0) Gecko/20100101 Firefox/136.0\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\r\n\
Accept-Encoding: gzip, deflate, br, zstd\r\n\
Referer: https://www.google.com/\r\n\
Alt-Used: www.youtube.com\r\n\
Connection: keep-alive\r\n\
Cookie: YSC=i3W6ivvNcHA; VISITOR_PRIVACY_METADATA=CgJGUhIcEhgSFhMLFBUWFwwYGRobHB0eHw4PIBAREiEgIw%3D%3D; PREF=tz=Europe.Paris; SOCS=CAISEwgDEgk3MzM4ODU4MTgaAmZyIAEaBgiA17i-Bg; VISITOR_INFO1_LIVE=60bvja2H2zI; __Secure-ROLLOUT_TOKEN=CJDp_OeDgvCNmwEQncjeuN3_iwMYncjeuN3_iwM%3D\r\n\
Upgrade-Insecure-Requests: 1\r\n\
Sec-Fetch-Dest: document\r\n\
Sec-Fetch-Mode: navigate\r\n\
Sec-Fetch-Site: cross-site\r\n\
Sec-Fetch-User: ?1\r\n\
Priority: u=0, i\r\n\
\r\n";

	std::string	req_avec_body = "POST /api/v1/users HTTP/1.1\r\n\
Host: example.com\r\n\
Content-Type: application/json\r\n\
Authorization: Bearer abcdef123456\r\n\
Content-Length: 342\r\n\
\r\n\
{\r\n\
  \"first_name\": \"Jean\",\r\n\
  \"last_name\": \"Dupont\",\r\n\
  \"email\": \"jean.dupont@example.com\",\r\n\
  \"password\": \"SuperSecret123!\",\r\n\
  \"address\": {\r\n\
    \"street\": \"123 Rue de Paris\",\r\n\
    \"city\": \"Paris\",\r\n\
    \"zip\": \"75001\",\r\n\
    \"country\": \"France\"\r\n\
  },\r\n\
  \"preferences\": {\r\n\
    \"newsletter\": true,\r\n\
    \"notifications\": [\"email\", \"sms\"]\r\n\
  }\r\n\
}";


	std::string	req_trunked_body = "POST /api/v1/users HTTP/1.1\r\n\
Host: example.com\r\n\
Content-Type: application/json\r\n\
Authorization: Bearer abcdef123456\r\n\
Transfer-Encoding: chunked\r\n\
\r\n\
27\r\n\
Voici les données du premier morceau\r\n\r\n\
1C\r\n\
et voici un second morceau\r\n\r\n\
20\r\n\
et voici deux derniers morceaux \r\n\
12\r\n\
sans saut de ligne\r\n\
0\r\n\
\r";

	RequestParser	req;
	std::ifstream	inf;
	std::string		buf;
	std::string		request;
	ssize_t			bytes_read;

	inf.open("test.txt");
	if (!inf.is_open())
	{
		std::cerr << "Error with infile" << std::endl;
		return (1);
	}

	int sockets[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
	{
		std::cerr << "Error creating socketpair" << std::endl;
		return (1);
	}

	buf.resize(BUFFER_SIZE);
	while (inf.read(&buf[0], BUFFER_SIZE) || inf.gcount() > 0)
	{
		send(sockets[0], buf.c_str(), inf.gcount(), 0);
	}
	inf.close();
	close(sockets[0]);
	buf.clear();
	buf.resize(BUFFER_SIZE);

	while (request.find("\r\n\r\n") == std::string::npos)
	{
		bytes_read = recv(sockets[1], &buf[0], BUFFER_SIZE, 0);
		if (bytes_read <= 0)
		{
			std::cerr << "erreur main" << std::endl;
			return (1);
		}
		request += buf;
	}

	// std::cout << "Requete dans le main: " << std::endl;
	// std::cout << request << std::endl << std::endl;

	req.parseRequest(request, sockets[1]);

	return (0);
}
