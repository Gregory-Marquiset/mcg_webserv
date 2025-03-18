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
	std::string	bad = "GET /?app=desktop&hl=FR HTTP/3\r\n";
	// std::string	POST_request;
	// std::string	DELETE_request;

	//std::cout << GET_request << std::endl << std::endl;


	RequestParser	req;

	req.parseRequest(GET_request, 0);
}
