/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:48:48 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/10 12:54:34 by cdutel           ###   ########.fr       */
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
		std::string		getMethod(void) const;
		std::string		getHTTP(void) const;
		std::string		getBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		bool			getCgi(void) const;
		bool			getAutoIndex(void) const;

	private:
		Server								*_serv_info;
		RequestParser						_request;
		ErrorManagement						*_error_class;
		LocationBlock						_location_to_use;

		//Stockage des infos utiles pour la réponse ou cgi
		std::string							_final_path;
		std::string							_method;
		std::string							_http_version;
		std::string							_request_body;
		std::map<std::string, std::string>	_headers;
		bool								_cgi;
		bool								_autoindex;

		void			processRequest(void);
		void			compareUriWithLocations(void);
		void			checkAllowedMethod(void);
		void			checkMaxBodySize(void);
		void			checkIfUriIsCgi(void);
		void			addRootPath(void);
		void			extractDirectoryContent(void);
		void			generateHTMLBody(std::map<std::string, std::string>	&directory_content);
};

#endif
