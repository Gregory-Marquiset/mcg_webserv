/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:54:07 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/06 17:57:59 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/response/ResponseMaker.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ResponseMaker::ResponseMaker(void)
{
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


/* ================= GETTERS ======================== */
std::string	ResponseMaker::getFinalResponse(void) const
{
	return (this->_final_response);
}
