#include "../../includes/response/ResponseMaker.hpp"

void ResponseMaker::createPostCGIResponse(void)
{
	std::string response;

	std::cout << "cgi status: " << this->_req_infos.getCgi() << std::endl;
	if (this->_req_infos.getCgi() == true)
	{
		std::string cgi_return;

		cgi_return = we_checkCGI(
			this->_req_infos.getCgiPath(),
			this->_req_infos.getFinalPath(),
			this->_req_infos.getCookie(),
			this->_req_infos.getBody(),
			this->_error_class
		);

		response += this->_req_infos.getHTTP() + " 200 OK\r\n";
		response += "Server: webserv\r\n";
		response += "Date: " + Utils::getTime(0) + " GMT\r\n";
		response += cgi_return;

		this->_final_response = response;
		return;
	}
	// Si jamais y a une erreur
	this->_error_class.setErrorCode(500);
	throw ResponseMaker::ResponseException("POST CGI ERROR");
}
