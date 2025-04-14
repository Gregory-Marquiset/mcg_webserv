/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:00:49 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/14 11:40:59 by cdutel           ###   ########.fr       */
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

		static std::string					getTime(int n);
		static std::string					findMIME(std::string path);
		static std::string					getErrorString(int error_code);
		static std::map<int, std::string>	get_error_map(void);
		static int							strtoi(std::string str);
		static int							authorizedMIME(std::string ext);
};

#endif
