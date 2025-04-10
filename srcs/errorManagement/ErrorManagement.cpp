/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorManagement.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:19:54 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/31 14:27:55 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/errorManagement/ErrorManagement.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ErrorManagement::ErrorManagement(void) : _error_code(0)
{
}

ErrorManagement::ErrorManagement(ErrorManagement const &copy)
{
	*this = copy;
}

ErrorManagement::~ErrorManagement(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
ErrorManagement	&ErrorManagement::operator=(ErrorManagement const &inst)
{
	if (this != &inst)
	{
		this->_error_code = inst._error_code;
	}
	return (*this);
}


/* ================= SETTERS ======================== */
void	ErrorManagement::setErrorCode(int error)
{
	this->_error_code = error;
}


/* ================= GETTERS ======================== */
int	ErrorManagement::getErrorCode(void) const
{
	return (this->_error_code);
}
