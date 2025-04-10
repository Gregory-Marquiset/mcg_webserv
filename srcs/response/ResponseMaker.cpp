/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:54:07 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/10 11:01:36 by cdutel           ###   ########.fr       */
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
	std::cout << std::endl << std::endl;
	try
	{
		if (this->_error_class.getErrorCode() != 0)
		{
			if (this->_error_class.getErrorCode() == 301 || this->_error_class.getErrorCode() == 302)
				this->createRedirectionResponse();
			else
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
				this->createDeleteResponse();
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
	std::string					response;
	std::map<int, std::string>	error_files = Utils::get_error_map();
	int							error_code = this->_error_class.getErrorCode();

	for (std::map<int, std::string>::iterator it = error_files.begin(); it != error_files.end(); it++)
	{
		if (error_code == it->first)
			error_path = it->second;
	}
	if (error_path.empty() || access(error_path.c_str(), F_OK) != 0 || access(error_path.c_str(), R_OK) != 0)
	{
		response += this->_req_infos.getHTTP() + " 500 Internal Server Error\r\n";
		response += "Server: webserv\r\n";
		response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
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
			response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
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

			response += this->_req_infos.getHTTP() + " " + Utils::getErrorString(error_code) + "\r\n";
			response += "Server: webserv\r\n";
			response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
			response += "Content-Type: " + Utils::findMIME(error_path) + "\r\n";
			response += "Content-Length: " + body_size + "\r\n";
			response += "\r\n";
			response += content.str();
		}
	}
	this->_final_response = response;
}

void	ResponseMaker::createRedirectionResponse(void)
{
	std::string	response;
	std::string	path = this->_req_infos.getFinalPath();

	response += this->_req_infos.getHTTP() + Utils::getErrorString(this->_error_class.getErrorCode());
	response += "Server: webserv\r\n";
	response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
	response += "Location: " + path + "\r\n";

	this->_final_response = response;
}

void	ResponseMaker::createAutoindexResponse(void)
{
	std::string	response;
	std::string	path = this->_req_infos.getFinalPath();

	response += this->_req_infos.getHTTP() + Utils::getErrorString(this->_error_class.getErrorCode());
	response += "Server: webserv\r\n";
	response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
	response += "Location: " + path + "\r\n";

	this->_final_response = response;
}

void	ResponseMaker::createGetResponse(void)
{
	std::string	response;

	if (this->_req_infos.getAutoIndex() == false)
	{
		if (this->_req_infos.getCgi() == true)
		{
			//on verra
			return;
		}

		std::string	path = this->_req_infos.getFinalPath();

		std::cout << "Path dans la reponse : " << path << std::endl;

		if (access(path.c_str(), F_OK) != 0)
		{
			this->_error_class.setErrorCode(404);
			throw ResponseMaker::ResponseException("Fichier inexistant");
		}
		if (access(path.c_str(), R_OK) != 0)
		{
			this->_error_class.setErrorCode(403);
			throw ResponseMaker::ResponseException("Accès au fichier interdit");
		}
		std::ifstream	file(path.c_str());

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
		response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
		response += "Content-Type: " + Utils::findMIME(this->_req_infos.getFinalPath()) + "\r\n";
		response += "Content-Length: " + body_size + "\r\n";
		response += "\r\n";
		response += content.str();
	}
	else
	{
		std::stringstream	size;
		std::string			body_size;

		size << this->_req_infos.getBody().size();
		body_size = size.str();

		response += this->_req_infos.getHTTP() + " 200 OK\r\n";
		response += "Server: webserv\r\n";
		response += "Date: " + Utils::getTime() + " GMT" + "\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content-Length: " + body_size + "\r\n";
		response += this->_req_infos.getBody();
	}

	this->_final_response = response;
}

void	ResponseMaker::createDeleteResponse(void)
{
	std::string		response;
	if (this->_req_infos.getCgi() == true)
	{
		//on verra
		return;
	}
	std::string path = this->_req_infos.getFinalPath();
	//path += this->_req_infos.getFinalPath();
	if (access(path.c_str(), F_OK) != 0)
	{
		this->_error_class.setErrorCode(404);
		throw ResponseMaker::ResponseException("Fichier inexistant");
	}
	if (access(path.c_str(), W_OK) != 0)
	{
		this->_error_class.setErrorCode(403);
		throw ResponseMaker::ResponseException("Accès au fichier interdit");
	}
	if (remove(path.c_str()) != 0)
	{
		this->_error_class.setErrorCode(500);
		throw ResponseMaker::ResponseException("Erreur lors de la suppresion du fichier");
	}

	response += this->_req_infos.getHTTP() + " 204 No Content\r\n";
	response += "Server: webserv\r\n";
	response += "Date: " + Utils::getTime() + " GMT" + "\r\n";

	this->_final_response = response;
}
