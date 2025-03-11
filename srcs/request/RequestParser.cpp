/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:41:37 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/11 11:03:55 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/RequestParser.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
RequestParser::RequestParser(void) : _actual_state(RequestParser::INIT), _error_state(RequestParser::NO_ERROR), _error_code(0), _escaped_char(ESCAPED_CHAR), _is_uri_cgi(false)
{
}

RequestParser::RequestParser(Server *serv) : _serv_info(serv), _actual_state(RequestParser::INIT), _error_state(RequestParser::NO_ERROR), _error_code(0), _escaped_char(ESCAPED_CHAR), _is_uri_cgi(false)
{
}

// RequestParser::RequestParser(std::string request, std::string root, std::string index) : _root_path(root), _index_path(index)
// {

// }

RequestParser::RequestParser(RequestParser const &copy)
{
	*this = copy;
}

RequestParser::~RequestParser(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
RequestParser	&RequestParser::operator=(RequestParser const &inst)
{
	if (this != &inst)
	{
	}
	return (*this);
}


/* ================= DEBUG ======================== */
// static void	printDebugMsg(std::string msg)
// {
// 	std::cerr << msg << std::endl;
// }

/* ================= GETTERS ======================== */


/* ================= SETTERS ======================== */
void	RequestParser::setErrorCode(int error)
{
	this->_error_code = error;
}

void	RequestParser::setFullRequest(const std::string request)
{
	this->_full_request = request;
}


/* ================= NON MEMBER FUNCTIONS ======================== */
static bool	is_non_printable(char c)
{
	return (!std::isprint(static_cast<unsigned char>(c)));
}

static bool	is_space(char c)
{
	return (std::isspace(static_cast<unsigned char>(c)));
}

static bool	is_value_valid(char c)
{
	if (c == ' ' || c == '\t')
		return (false);
	if (!std::isprint(static_cast<unsigned char>(c)))
		return (true);
	if (std::isspace(static_cast<unsigned char>(c)))
		return (true);
	return (false);
}

/* ================= PUBLIC MEMBER FUNCTIONS ======================== */
void	RequestParser::parseRequest(const std::string request)
{
	if (request.empty())
	{
		this->_actual_state = RequestParser::FATAL_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		return ;
	}
	this->setFullRequest(request);
	try
	{
		parseRequestLine(this->_full_request);
		parseHeaders(this->_full_request);
		//check si req.empty(), doit etre si get ou delete, sinon parseBody
		//parseBody(this->_full_request);
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << req_exc.what() << std::endl;
	}
}


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	RequestParser::parseRequestLine(std::string &req)
{
	this->parseMethod(req);
	this->parseURI(req);
	this->parseHTTP(req);
}

void	RequestParser::parseMethod(std::string &req)
{
	std::string	method;
	int			space_pos = 0;

	this->_actual_state = RequestParser::METHOD;
	space_pos = req.find_first_of(" ");
	method = req.substr(0, space_pos);
	if (method != "GET" && method != "POST" && method != "DELETE")			//A changer, a verifier dans les infos du serveur
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::METHOD_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(405);
		//std::cout << "*" << method << "*" << std::endl;
		throw RequestParser::RequestException("Invalid Method");
	}
	this->_request_method = method;
	req.erase(0, space_pos + 1);

	// std::cout << "*" << method << "*" << std::endl;
	// std::cout << "*" << req << "*" << std::endl;
}

void	RequestParser::parseURI(std::string &req)			//Rajouter une vérif pour voir si l'uri correspond à un cgi du serveur
{
	std::string	uri;
	std::string	check_encoded;
	int			space_pos = 0;

	this->_actual_state = RequestParser::URI;
	if (req.find_first_of(" ") == std::string::npos)
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::URI_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid URI 1");
	}
	space_pos = req.find_first_of(" ");
	uri = req.substr(0, space_pos);
	req.erase(0, space_pos + 1);
	if (uri[0] != '/' || uri.find_first_not_of(VALID_CHARSET) != std::string::npos)
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::URI_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid URI 2");
	}
	for (size_t i = 0; i < uri.size(); i++)
	{
		if (uri[i] == '%')
		{
			if (i + 2 >= uri.size())
			{
				if (this->_error_state == RequestParser::NO_ERROR)
					this->_error_state = RequestParser::URI_ERROR;
				if (this->_error_code == 0)
					this->setErrorCode(400);
				throw RequestParser::RequestException("Invalid URI 3");
			}
			check_encoded = uri.substr(i, 3);
			//std::cout << check_encoded << std::endl;
			if (this->_escaped_char.find(check_encoded) == std::string::npos)
			{
				if (this->_error_state == RequestParser::NO_ERROR)
					this->_error_state = RequestParser::URI_ERROR;
				if (this->_error_code == 0)
					this->setErrorCode(400);
				throw RequestParser::RequestException("Invalid URI 4");
			}
			i += 2;
		}
	}
	this->_request_uri = uri;
}

void	RequestParser::parseHTTP(std::string &req)
{
	std::string	http;
	int			i;
	long		n;

	this->_actual_state = RequestParser::HTTP;
	if (req.find("HTTP/") != 0)
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	http += req.substr(0, 5);
	req.erase(0, 5);
	i = 0;
	if (!(req[i] >= '0' && req[i] <= '9'))
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	while (req[i] && (req[i] >= '0' && req[i] <= '9'))
		i++;
	n = std::atoi(req.c_str());
	if (n < 1 || n > std::numeric_limits<int>::max())
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	http += req.substr(0, i);
	req.erase(0, i);
	i = 0;
	if (req[i] == '\r')
	{
		req.erase(0, 2);
		return;
	}
	else if (req[i] != '.' || !(req[i + 1] >= '0' && req[i + 1] <= '9'))
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	http += ".";
	req.erase(0, 1);
	while (req[i] && (req[i] >= '0' && req[i] <= '9'))
		i++;
	n = std::atoi(req.c_str());
	if (n < 0 || n > std::numeric_limits<int>::max())
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	http += req.substr(0, i);
	req.erase(0, i);
	if (req[0] != '\r' && req[1] != '\n')
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HTTP_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid HTTP");
	}
	req.erase(0, 2);
}

// void	RequestParser::printMap(void)
// {
//  	for (std::map<std::string, std::string>::iterator it = this->_request_headers.begin(); it != this->_request_headers.end(); it++)
// 	{
// 		std::cout << "*" << it->first << ": " << it->second << "*" << std::endl;
// 	}
// }

void	RequestParser::parseHeaders(std::string &req)
{
	std::string::iterator	it;
	std::string::iterator	its;
	std::string				temp_key;
	std::string				temp_value;
	size_t					pos = 0;
	size_t					end = 0;

	this->_actual_state = RequestParser::HEADERS;
	if (req.find("\r\n\r\n") == std::string::npos)
	{
		if (this->_error_state == RequestParser::NO_ERROR)
			this->_error_state = RequestParser::HEADERS_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		throw RequestParser::RequestException("Invalid Headers end");
	}
	while (!req.empty())
	{
		end = req.find("\r\n");
		if (end == 0)
			break;
		pos = req.find_first_of(":");
		if (pos == std::string::npos || pos > end)
		{
			if (this->_error_state == RequestParser::NO_ERROR)
				this->_error_state = RequestParser::HEADERS_ERROR;
			if (this->_error_code == 0)
				this->setErrorCode(400);
			throw RequestParser::RequestException("Invalid Headers Key");
		}
		temp_key = req.substr(0, pos);
		it = std::find_if(temp_key.begin(), temp_key.end(), is_non_printable);
		its = std::find_if(temp_key.begin(), temp_key.end(), is_space);
		if (it != temp_key.end() || its != temp_key.end())
		{
			if (this->_error_state == RequestParser::NO_ERROR)
				this->_error_state = RequestParser::HEADERS_ERROR;
			if (this->_error_code == 0)
				this->setErrorCode(400);
			throw RequestParser::RequestException("Invalid Headers Key2");
		}
		if (req.find_first_not_of(" \t", pos + 1) == pos + 1)
			temp_value = req.substr(pos + 1, end - pos - 1);
		else if (req.find_first_not_of(" \t", pos + 1) == end)
			temp_value = "";
		else
		{
			pos = req.find_first_not_of(" \t", pos + 1);
			temp_value = req.substr(pos, end - pos);
		}
		its = std::find_if(temp_value.begin(), temp_value.end(), is_value_valid);
		if (its != temp_value.end())
		{
			std::cout << "*" << temp_value << "*" << std::endl;
			throw RequestParser::RequestException("Invalid Headers Value");
		}
		this->_request_headers.insert(std::make_pair(temp_key, temp_value));
		req.erase(0, end + 2);
	}
	for (std::map<std::string, std::string>::iterator it = this->_request_headers.begin(); it != this->_request_headers.end(); it++)
	{
		std::cout << "*" << it->first << ": " << it->second << "*" << std::endl;
	}
	req.erase(0, 2);
}
