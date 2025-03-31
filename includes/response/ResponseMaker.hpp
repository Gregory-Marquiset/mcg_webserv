/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:46:03 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/31 14:45:06 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEMAKER_HPP
# define RESPONSEMAKER_HPP

# include "../WebServ.hpp"
# include "../../includes/errorManagement/ErrorManagement.hpp"

class	ResponseMaker
{
	public:
		ResponseMaker(void);
		ResponseMaker(ErrorManagement &err);
		ResponseMaker(ResponseMaker const &copy);
		~ResponseMaker(void);

		ResponseMaker	&operator=(ResponseMaker const &inst);

		std::string		getFinalResponse(void) const;

	private:
		ErrorManagement	_error_class;
		std::string		_final_response;
};

#endif
