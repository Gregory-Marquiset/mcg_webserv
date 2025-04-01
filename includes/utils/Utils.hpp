/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:00:49 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/01 14:21:52 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "../WebServ.hpp"

class	Utils
{
	public:
		Utils(void);
		Utils(Utils const &copy);
		~Utils(void);

		Utils		&operator=(Utils const &inst);

		static std::string	getTime(void);
		static std::string	findMIME(std::string path);
};

#endif
