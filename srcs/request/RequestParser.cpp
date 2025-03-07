/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:41:37 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/07 15:45:09 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/RequestParser.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
RequestParser::RequestParser(void)
{
}

RequestParser::RequestParser(const std::string request) : _actual_state(RequestParser::INIT), _error_state(RequestParser::NO_ERROR), _error_code(0) , _full_request(request), _escaped_char(ESCAPED_CHAR)
{
	if (request.empty())
	{
		this->_actual_state = RequestParser::FATAL_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		return ;
	}
	try
	{
		parseRequestLine(this->_full_request);
		//parseHeaders(this->_full_request);
		//parseBody(this->_full_request);
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << req_exc.what() << std::endl;
	}
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

/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	RequestParser::parseRequestLine(std::string &req)
{
	parseMethod(req);
	parseURI(req);
}

void	RequestParser::parseMethod(std::string &req)
{
	std::string	method;
	int			space_pos = 0;

	this->_actual_state = RequestParser::METHOD;
	space_pos = req.find_first_of(" ");
	method = req.substr(0, space_pos);
	if (method != "GET" && method != "POST" && method != "DELETE")
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

void	RequestParser::parseURI(std::string &req)
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
