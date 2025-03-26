/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:48:48 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/25 17:45:31 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESSREQUEST_HPP
# define PROCESSREQUEST_HPP

//# include "WebServ.hpp"
# include <algorithm>
# include <cstdlib>
# include <iostream>
# include <limits>
# include <map>
# include <stdexcept>
# include <string>
# include <vector>
# include "RequestParser.hpp"

class	ProcessRequest
{
	public:
		ProcessRequest(void);
		ProcessRequest(Server *serv, RequestParser &req);
		ProcessRequest(ProcessRequest const &copy);
		~ProcessRequest(void);

		ProcessRequest	&operator=(ProcessRequest const &inst);

	private:
		Server			*_serv_info;
		RequestParser	_request;
		LocationBlock	_location_to_use;

		std::string		_final_path;

		void			processRequest(void);
		void			compareUriWithLocations(void);
		//void			checkAllowedMethod(void);
		void			addRootPath(void);
};

#endif
