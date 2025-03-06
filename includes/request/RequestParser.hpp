/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:34:20 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/06 20:08:42 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "WebServ.hpp"

class	RequestParser
{
	public:
		RequestParser(void);
		RequestParser(const std::string request);
		//RequestParser(std::string request, std::string root, std::string index);
		RequestParser(RequestParser const &copy);
		~RequestParser(void);

		RequestParser	&operator=(RequestParser const &inst);

		enum	e_parse_state
		{
			INIT,
			FINAL,
			ERROR,
			FATAL_ERROR
		};

	private:
		int				_actual_state;
		int				_error_code;
		std::string		_full_request;
		// Struct de la REQUEST
		// Request-Line
		std::string		_request_method;	//GET, POST, DELETE....
		std::string		_request_uri;		//path de la requete
		std::string		_request_http_version;
		// Headers
		std::map<std::string, std::string>	_request_headers;
		// Body
		std::string		_request_body;

		//Parsing de Request-Line
		void			parseRequestLine(std::string &req);
		void			parseMethod(std::string &req);
		void			parseURI(std::string &req);
		void			parseHTTP(std::string &req);

		//Parsing des Headers
		void			parseHeaders(std::string &req);

		//Parsing du body
		void			parseBody(std::string &req);

		void			setErrorCode(int error);
};

#endif
