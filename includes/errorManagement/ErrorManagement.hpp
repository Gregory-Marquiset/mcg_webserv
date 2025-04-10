/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorManagement.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:07:01 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/31 14:15:57 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORMANAGEMENT_HPP
# define ERRORMANAGEMENT_HPP

# include "../../includes/WebServ.hpp"

class	ErrorManagement
{
	public:
		ErrorManagement(void);
		ErrorManagement(ErrorManagement const &copy);
		~ErrorManagement(void);

		ErrorManagement	&operator=(ErrorManagement const &inst);

		void			setErrorCode(int error);

		int				getErrorCode(void) const;

	private:
		int	_error_code;
};

#endif
