/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:54:07 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/01 15:42:08 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ResponseMaker::ResponseMaker(void)
{
}

ResponseMaker::ResponseMaker(ErrorManagement &err, ProcessRequest &req_infos) : _error_class(err), _req_infos(req_infos)
{
	std::cout << "Constructeur ResponseMaker :" << std::endl;
	std::cout << "Method : " << this->_req_infos.getMethod() << std::endl;
	std::cout << "HTTP version : " << this->_req_infos.getHTTP() << std::endl;
	std::cout << "Final path : " << this->_req_infos.getFinalPath() << std::endl;
	std::cout << std::endl << std::endl;
	try
	{
		if (this->_error_class.getErrorCode() != 0)
		{
			this->createErrorResponse();
		}
		else
		{
			if (_req_infos.getMethod() == "GET")
			{
				this->createGetResponse();
			}
			else if (_req_infos.getMethod() == "POST")
			{

			}
			else if (_req_infos.getMethod() == "DELETE")
			{

			}
		}
	}
	catch (ResponseMaker::ResponseException &e)
	{
		std::cerr << e.what() << std::endl;
		this->createErrorResponse();
	}
}

ResponseMaker::ResponseMaker(ResponseMaker const &copy)
{
	*this = copy;
}

ResponseMaker::~ResponseMaker(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
ResponseMaker	&ResponseMaker::operator=(ResponseMaker const &inst)
{
	if (this != &inst)
	{

	}
	return (*this);
}

/* ================= SETTERS ======================== */


/* ================= GETTERS ======================== */
std::string	ResponseMaker::getFinalResponse(void) const
{
	return (this->_final_response);
}


/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	ResponseMaker::createErrorResponse(void)
{
	std::string					error_path;
	std::string					alt_path;
	std::string					response;
	std::map<int, std::string>	error_files = Utils::get_error_map();
	int							error_code = this->_error_class.getErrorCode();

	for (std::map<int, std::string>::iterator it = error_files.begin(); it != error_files.end(); it++)
	{
		if (error_code == it->first)
			error_path = it->second;
	}
	alt_path += "../." + error_path;
	if (error_path.empty() || !access(alt_path.c_str(), F_OK) || !access(alt_path.c_str(), F_OK))
	{
		response += this->_req_infos.getHTTP() + " 500 Internal Server Error\r\n";
		response += "Server: webserv\r\n";
		response += "Date: " + Utils::getTime() + "GMT" + "\r\n";
		response += "Content-Type: text/plain\r\n";
		response += "Content-Length: 26\r\n";
		response += "\r\n";
		response += "500 Internal Server Error";
	}
	else
	{
		std::ifstream	file(error_path.c_str());

		if (!file.is_open())
		{
			response += this->_req_infos.getHTTP() + " 500 Internal Server Error\r\n";
			response += "Server: webserv\r\n";
			response += "Date: " + Utils::getTime() + "GMT" + "\r\n";
			response += "Content-Type: text/plain\r\n";
			response += "Content-Length: 26\r\n";
			response += "\r\n";
			response += "500 Internal Server Error";
		}
		else
		{
			std::stringstream	content;
			std::stringstream	size;
			std::string			body;
			std::string			body_size;

			content << file.rdbuf();
			body = content.str();
			size << body.size();
			body_size = size.str();

			response += this->_req_infos.getHTTP() + " " + Utils::getErrorString(error_code);
			response += "Server: webserv\r\n";
			response += "Date: " + Utils::getTime() + "GMT" + "\r\n";
			response += "Content-Type: " + Utils::findMIME(this->_req_infos.getFinalPath()) + "\r\n";
			response += "Content-Length: " + body_size + "\r\n";
			response += "\r\n";
			response += content.str();
		}
	}
	this->_final_response = response;
}

void	ResponseMaker::createGetResponse(void)
{
	std::string		response;
	if (this->_req_infos.getCgi() == true)
	{
		//on verra
		return;
	}
	std::string path = "../.";
	path += this->_req_infos.getFinalPath();
	if (!access(path.c_str(), F_OK))
	{
		this->_error_class.setErrorCode(404);
		throw ResponseMaker::ResponseException("Fichier inexistant");
	}
	else if (!access(path.c_str(), R_OK))
	{
		this->_error_class.setErrorCode(403);
		throw ResponseMaker::ResponseException("Accès au fichier interdit");
	}
	std::ifstream	file(this->_req_infos.getFinalPath().c_str());

	if (!file.is_open())
	{
		this->_error_class.setErrorCode(404);
		throw ResponseMaker::ResponseException("Fichier non ouvert");
	}
	std::stringstream	content;
	std::stringstream	size;
	std::string			body;
	std::string			body_size;

	content << file.rdbuf();
	body = content.str();
	size << body.size();
	body_size = size.str();

	response += this->_req_infos.getHTTP() + " 200 OK\r\n";
	response += "Server: webserv\r\n";
	response += "Date: " + Utils::getTime() + "GMT" + "\r\n";
	response += "Content-Type: " + Utils::findMIME(this->_req_infos.getFinalPath()) + "\r\n";
	response += "Content-Length: " + body_size + "\r\n";
	response += "\r\n";
	response += content.str();

	this->_final_response = response;
}
