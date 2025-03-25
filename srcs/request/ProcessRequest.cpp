/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:01:57 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/25 17:51:13 by cdutel           ###   ########.fr       */
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


/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	ProcessRequest::processRequest(void)
{
	try
	{
		this->compareUriWithLocations();
		//this->checkAllowedMethod();
		this->addRootPath();
	}
	catch(...)
	{

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
	locations = this->_serv_info->getServerBlock().getLocation();

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

// void	ProcessRequest::checkAllowedMethod(void)
// {

// }

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
}
