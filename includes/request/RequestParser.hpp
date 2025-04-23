/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:34:20 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/17 05:00:03 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "../../includes/WebServ.hpp"
# include "../epollManager/EPollManager.hpp"
# include "../errorManagement/ErrorManagement.hpp"

# define ESCAPED_CHAR "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F\
%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D%1E%1F\
%20%22%23%25%26%27%28%29%2A%2B%2C%2F%3A%3B%3C%3D\
%3E%3F%40%5B%5C%5D%5E%5F%60%7B%7C%7D"
# define VALID_CHARSET "!$%&'()*+,-./0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~"
# define BUFFER_SIZE 100000

class	RequestParser
{
	public:
		RequestParser(void);
		RequestParser(ErrorManagement &err);
		RequestParser(RequestParser const &copy);
		~RequestParser(void);

		RequestParser	&operator=(RequestParser const &inst);

		void			setIsCgi(bool value);

		std::string		getMethod(void) const;
		std::string		getURI(void) const;
		std::string		getHTTP(void) const;
		std::string		getBody(void) const;
		bool			getIsCgi(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		std::string		getConnection(void) const;
		std::string		getCookie(void) const;
		std::string		getContentType(void) const;

		void			parseRequest(const std::string request, int clientFd);

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
		ErrorManagement	*_error_class;
		std::string		_full_request;
		std::string		_escaped_char;
		bool			_is_uri_cgi;

		// Stockage des infos utile dans les headers
		std::string		_content_type;
		std::string		_connection;		//Pour close le fd ou pas
		std::string		_cookie;			//Pour greg
		unsigned long	_content_length;
		bool			_cnt_lenght;
		bool			_transfert_encoding;

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
		void			validateHeaders(void);

		//Parsing du body
		void			parseBody(std::string &req, int clientFd);

		//Random
		void			setFullRequest(const std::string request);
};

#endif
