/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:46:03 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/06 17:51:45 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEMAKER_HPP
# define RESPONSEMAKER_HPP

# include "../WebServ.hpp"

class	ResponseMaker
{
	public:
		ResponseMaker(void);
		ResponseMaker(ResponseMaker const &copy);
		~ResponseMaker(void);

		ResponseMaker	&operator=(ResponseMaker const &inst);

		std::string		getFinalResponse(void) const;

	private:
		std::string		_final_response;
};

#endif
