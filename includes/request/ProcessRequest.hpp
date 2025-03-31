/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:48:48 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/31 14:40:14 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESSREQUEST_HPP
# define PROCESSREQUEST_HPP

# include "../../includes/WebServ.hpp"
# include "../../includes/errorManagement/ErrorManagement.hpp"
# include "RequestParser.hpp"

class	ProcessRequest
{
	public:
		ProcessRequest(void);
		ProcessRequest(Server *serv, RequestParser &req, ErrorManagement &err);
		ProcessRequest(ProcessRequest const &copy);
		~ProcessRequest(void);

		ProcessRequest	&operator=(ProcessRequest const &inst);

		std::string		getFinalPath(void) const;

	private:
		Server			*_serv_info;
		RequestParser	_request;
		ErrorManagement	_error_class;
		LocationBlock	_location_to_use;

		std::string		_final_path;

		void			processRequest(void);
		void			compareUriWithLocations(void);
		void			checkAllowedMethod(void);
		void			checkMaxBodySize(void);
		void			checkIfUriIsCgi(void);
		void			addRootPath(void);
};

#endif
