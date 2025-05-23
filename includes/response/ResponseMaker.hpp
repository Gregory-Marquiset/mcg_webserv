/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMaker.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmarquis <gmarquis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:46:03 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/26 00:03:09 by gmarquis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEMAKER_HPP
# define RESPONSEMAKER_HPP

# include "../WebServ.hpp"
# include "../../includes/errorManagement/ErrorManagement.hpp"
# include "../../includes/request/ProcessRequest.hpp"
# include "../../includes/utils/Utils.hpp"
# include "../../includes/cgi/cgi.hpp"

class	ResponseMaker
{
	public:
		ResponseMaker(void);
		ResponseMaker(ErrorManagement &err, ProcessRequest &req_infos);
		ResponseMaker(ResponseMaker const &copy);
		~ResponseMaker(void);

		ResponseMaker	&operator=(ResponseMaker const &inst);

		std::string		getFinalResponse(void) const;

		class	ResponseException : public std::exception
		{
			public:
				ResponseException(const std::string &msg) throw() : _msg(msg) {}
				virtual ~ResponseException() throw() {}

				virtual const char* what() const throw()
				{
					return _msg.c_str();
				}
			private:
				std::string	_msg;
		};
		void	createPostCGIResponse(void);

	private:
		ErrorManagement	_error_class;
		ProcessRequest	_req_infos;
		std::string		_final_response;

		void			createErrorResponse(void);
		void			createRedirectionResponse(void);
		void			createAutoindexResponse(void);
		void			createGetResponse(void);
		void			createDeleteResponse(void);
		void			createPostResponse(void);
};



#endif
