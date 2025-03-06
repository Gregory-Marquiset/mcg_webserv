/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:41:37 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/06 20:21:56 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/RequestParser.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
RequestParser::RequestParser(void)
{
}

RequestParser::RequestParser(const std::string request) : _full_request(request), _actual_state(RequestParser::INIT), _error_code(0)
{
	if (request.empty())
	{
		this->_actual_state = RequestParser::FATAL_ERROR;
		if (this->_error_code == 0)
			this->setErrorCode(400);
		return ;
	}
	parseRequestLine(this->_full_request);
	parseHeaders(this->_full_request);
	parseBody(this->_full_request);
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

/* ================= GETTERS ======================== */


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */

