/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:34:20 by cdutel            #+#    #+#             */
/*   Updated: 2025/03/11 09:18:05 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <map>
//# include "WebServ.hpp"
#include "../epollManager/EPollManager.hpp"

# define ESCAPED_CHAR "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F\
%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D%1E%1F\
%20%22%23%25%26%27%28%29%2A%2B%2C%2F%3A%3B%3C%3D\
%3E%3F%40%5B%5C%5D%5E%5F%60%7B%7C%7D"

# define VALID_CHARSET "!$%&'()*+,-./0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~"

class	RequestParser
{
	public:
		RequestParser(void);
		// RequestParser(const std::string request);
		RequestParser(Server *serv);
		RequestParser(RequestParser const &copy);
		~RequestParser(void);

		RequestParser	&operator=(RequestParser const &inst);

		void			parseRequest(const std::string request);

		enum	e_parse_state
		{
			INIT,
			METHOD,
			URI,
			HTTP,
			HEADERS,
			BODY,
			FINAL
		};

		enum	e_error_state
		{
			NO_ERROR,
			METHOD_ERROR,
			URI_ERROR,
			HTTP_ERROR,
			REQUEST_LINE_ERROR,
			HEADERS_ERROR,
			BODY_ERROR,
			FATAL_ERROR
		};

		class	RequestException : public std::exception
		{
			public:
				RequestException(const std::string &msg) throw() : _msg(msg) {}
				virtual ~RequestException() throw() {}

				virtual const char* what() const throw()
				{
					return _msg.c_str();
				}
			private:
				std::string	_msg;
		};

	private:
		Server			*_serv_info;
		int				_actual_state;
		int				_error_state;
		int				_error_code;
		std::string		_full_request;
		std::string		_escaped_char;
		bool			_is_uri_cgi;
		// Struct de la REQUEST
		// Request-Line
		std::string		_request_method;		//GET, POST, DELETE....
		std::string		_request_uri;			//path de la requete
		std::string		_request_http_version;	//version HTTP
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
		//void			parseBody(std::string &req);

		void			setErrorCode(int error);
		void			setFullRequest(const std::string request);

		void			printMap(void);
};

#endif
