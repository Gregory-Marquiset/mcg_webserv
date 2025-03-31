/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:01:57 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/31 11:26:16 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/ProcessRequest.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ProcessRequest::ProcessRequest(void)
{
}

ProcessRequest::ProcessRequest(Server *serv, RequestParser &req) : _serv_info(serv), _request(req)
{
	this->processRequest();
}

ProcessRequest::ProcessRequest(ProcessRequest const &copy)
{
	*this = copy;
}

ProcessRequest::~ProcessRequest(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
ProcessRequest	&ProcessRequest::operator=(ProcessRequest const &inst)
{
	if (this != &inst)
	{

	}
	return (*this);
}


/* ================= SETTERS ======================== */


/* ================= GETTERS ======================== */
std::string	ProcessRequest::getFinalPath(void) const
{
	return (this->_final_path);
}

/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	ProcessRequest::processRequest(void)
{
	try
	{
		this->compareUriWithLocations();
		this->checkAllowedMethod();
		this->checkMaxBodySize();
		this->addRootPath();
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << req_exc.what() << std::endl;
	}
}

void	ProcessRequest::compareUriWithLocations(void)
{
	std::vector<LocationBlock>::iterator	it;
	std::vector<LocationBlock>				locations;
	std::string								uri;
	std::string								location_path;
	int										uri_size;
	int										location_path_size;
	int										last_location_match_size = -1;

	uri = this->_request.getURI();
	uri_size = uri.size();
	locations = this->_serv_info->getServerBlock().getLocationBlock();

	for (it = locations.begin(); it != locations.end(); it++)
	{
		location_path = it->getPath();
		if (location_path == "/" && last_location_match_size == -1)
			this->_location_to_use = *it;
		location_path_size = location_path.size();
		if (uri.compare(0, location_path_size, location_path) == 0)
		{
			if (uri_size == location_path_size || uri[location_path_size] == '/')
			{
				if (location_path_size > last_location_match_size)
				{
					last_location_match_size = location_path_size;
					this->_location_to_use = *it;
				}
			}
		}
	}
}

void	ProcessRequest::checkAllowedMethod(void)
{
	std::vector<std::string>	methods;

	methods = this->_location_to_use.getAllowMethods();
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (this->_request.getMethod() == *it)
			return ;
	}
	throw RequestParser::RequestException("Method not allowed in this location");
}

void	ProcessRequest::checkMaxBodySize(void)
{
	std::string	str_max_body_size = this->_location_to_use.getClientMaxBodySize();
	size_t		pos;
	size_t		multiplier = 0;
	size_t		max_body_size = 0;

	//std::cout << "str max body: " << str_max_body_size << std::endl;
	if (str_max_body_size.empty())
		str_max_body_size = "1M";
	pos = str_max_body_size.find_first_not_of("0123456789");
	if (pos != std::string::npos)
	{
		if ((str_max_body_size[pos] == 'k' || str_max_body_size[pos] == 'K') && pos == str_max_body_size.size() - 1)
		{
			multiplier += 1000;
		}
		else if ((str_max_body_size[pos] == 'm' || str_max_body_size[pos] == 'M') && pos == str_max_body_size.size() - 1)
		{
			multiplier += 1000000;
		}
		else
		{
			if (this->_request.getErrorCode() == 0)
				this->_request.setErrorCode(411);
			throw RequestParser::RequestException("Error with Max Body size in config");
		}
	}
	std::istringstream	iss(str_max_body_size);
	iss >> max_body_size;
	//std::cout << "max body size before mult: " << max_body_size << std::endl;
	if (multiplier != 0)
		max_body_size *= multiplier;
	//std::cout << "max body size after mult: " << max_body_size << std::endl;
	if (this->_request.getBody().size() > max_body_size)
	{
		if (this->_request.getErrorCode() == 0)
			this->_request.setErrorCode(413);
		throw RequestParser::RequestException("Body size is bigger than client max body size");
	}
}

void	ProcessRequest::checkIfUriIsCgi(void)
{
	std::vector<CgiHandler>	cgi_ext;
	std::string				uri;
	std::string				extension;
	size_t					pos;

	cgi_ext = this->_location_to_use.getCgiExtension();
	uri = this->_request.getURI();
	pos = uri.rfind(".");
	if (pos == std::string::npos)
		return ;
	extension = uri.substr(pos, std::string::npos);
	for (std::vector<CgiHandler>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
	{
		if (extension == it->getKey())
		{
			this->_request.setIsCgi(true);
			return ;
		}
	}
}

void	ProcessRequest::addRootPath(void)
{
	std::string	uri;
	size_t		path_size;

	uri = this->_request.getURI();
	uri.erase(0, this->_location_to_use.getPath().size());
	this->_final_path += this->_location_to_use.getRoot();
	this->_final_path += uri;

	//checker si cgi

	path_size = this->_final_path.size();
	if (this->_final_path[path_size - 1] == '/')
	{
		this->_final_path += this->_location_to_use.getIndex();
	}
	std::cout << this->_final_path << std::endl;
}
