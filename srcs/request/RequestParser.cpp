/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:41:37 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/24 15:45:04 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/RequestParser.hpp"
#include "../../includes/configFile/LocationBlock.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
RequestParser::RequestParser(void) : _escaped_char(ESCAPED_CHAR),
_content_length(0), _cnt_lenght(0), _transfert_encoding(0)
{
}

RequestParser::RequestParser(ErrorManagement &err) : _error_class(&err), _escaped_char(ESCAPED_CHAR), _content_length(0), _cnt_lenght(0), _transfert_encoding(0)
{
}

RequestParser::RequestParser(RequestParser const &copy)
{
	*this = copy;
}

RequestParser::~RequestParser(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
RequestParser	&RequestParser::operator=(RequestParser const &inst)
{
	if (this != &inst)
	{
		this->_error_class = inst._error_class;
		this->_full_request = inst._full_request;
		this->_escaped_char = inst._escaped_char;
		this->_content_type = inst._content_type;
		this->_connection = inst._connection;
		this->_cookie = inst._cookie;
		this->_content_length = inst._content_length;
		this->_cnt_lenght = inst._cnt_lenght;
		this->_transfert_encoding = inst._transfert_encoding;
		this->_request_method = inst._request_method;
		this->_request_uri = inst._request_uri;
		this->_request_http_version = inst._request_http_version;
		this->_request_headers = inst._request_headers;
		this->_request_body = inst._request_body;
	}
	return (*this);
}


/* ================= DEBUG ======================== */


/* ================= SETTERS ======================== */
void	RequestParser::setFullRequest(const std::string request)
{
	this->_full_request = request;
}


/* ================= GETTERS ======================== */
std::string	RequestParser::getMethod(void) const
{
	return (this->_request_method);
}

std::string	RequestParser::getURI(void) const
{
	return (this->_request_uri);
}

std::string	RequestParser::getHTTP(void) const
{
	return (this->_request_http_version);
}

std::string	RequestParser::getBody(void) const
{
	return (this->_request_body);
}

std::map<std::string, std::string>	RequestParser::getHeaders(void) const
{
	return (this->_request_headers);
}

std::string	RequestParser::getConnection(void) const
{
	return (this->_connection);
}

std::string	RequestParser::getCookie(void) const
{
	return (this->_cookie);
}

std::string	RequestParser::getContentType(void) const
{
	return (this->_content_type);
}

/* ================= NON MEMBER FUNCTIONS ======================== */
static bool	is_non_printable(char c)
{
	return (!std::isprint(static_cast<unsigned char>(c)));
}

static bool	is_space(char c)
{
	return (std::isspace(static_cast<unsigned char>(c)));
}

static bool	is_value_valid(char c)
{
	if (c == ' ' || c == '\t')
		return (false);
	if (!std::isprint(static_cast<unsigned char>(c)))
		return (true);
	if (std::isspace(static_cast<unsigned char>(c)))
		return (true);
	return (false);
}

static long	extract_size(std::string &str_chunk_size)
{
	if (str_chunk_size.empty())
		return (-1);
	for (size_t i = 0; i < str_chunk_size.size(); i++)
	{
		if (!std::isxdigit(str_chunk_size[i]))
			return (-1);
	}
	std::istringstream	iss(str_chunk_size);
	long				chunk_size;

	iss >> std::hex >> chunk_size;
	str_chunk_size.clear();
	return (chunk_size);
}


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */
void	RequestParser::parseRequest(const std::string request, int clientFd)
{
	std::cout << "\033[32mPARSING THE REQUEST\033[0m" << std::endl;
	if (request.empty())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		return ;
	}
	this->setFullRequest(request);
	try
	{
		parseRequestLine(this->_full_request);
		parseHeaders(this->_full_request);
		validateHeaders();

		//si la méthode de la demande n’inclut aucune sémantique définie pour un
		//corps d’entité, le corps de message DEVRAIT alors être ignoré lors du traitement de la demande.
		if (this->_request_method == "POST")
		{
			parseBody(this->_full_request, clientFd);
			std::cout << "\033[32mBody has been parsed\033[0m" << std::endl << std::endl;
		}
		std::cout << "Valid Request" << std::endl;
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << "\033[31m" << req_exc.what() << "\033[0m" << std::endl;
	}
}


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	RequestParser::parseRequestLine(std::string &req)
{
	this->parseMethod(req);
	this->parseURI(req);
	this->parseHTTP(req);
}

void	RequestParser::parseMethod(std::string &req)
{
	std::string	method;
	int			space_pos = 0;

	space_pos = req.find_first_of(" ");
	method = req.substr(0, space_pos);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(405);
		throw RequestParser::RequestException("INVALID METHOD");
	}
	this->_request_method = method;
	req.erase(0, space_pos + 1);
}

void	RequestParser::parseURI(std::string &req)
{
	std::string	uri;
	std::string	check_encoded;
	int			space_pos = 0;

	if (req.find_first_of(" ") == std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID URI");
	}
	space_pos = req.find_first_of(" ");
	uri = req.substr(0, space_pos);
	req.erase(0, space_pos + 1);
	if (uri[0] != '/' || uri.find_first_not_of(VALID_CHARSET) != std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID URI");
	}
	for (size_t i = 0; i < uri.size(); i++)
	{
		if (uri[i] == '%')
		{
			if (i + 2 >= uri.size())
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(400);
				throw RequestParser::RequestException("INVALID CHAR IN URI");
			}
			check_encoded = uri.substr(i, 3);
			if (this->_escaped_char.find(check_encoded) == std::string::npos)
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(400);
				throw RequestParser::RequestException("INVALID CHAR IN URI");
			}
			i += 2;
		}
	}
	this->_request_uri = uri;
}

void	RequestParser::parseHTTP(std::string &req)
{
	std::string	http;
	int			i;
	long		n;
	
	if (req.find("HTTP/") != 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	http += req.substr(0, 5);
	req.erase(0, 5);
	i = 0;
	if (!(req[i] >= '0' && req[i] <= '9'))
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	while (req[i] && (req[i] >= '0' && req[i] <= '9'))
		i++;
	n = std::atoi(req.c_str());
	if (n < 1 || n > std::numeric_limits<int>::max())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	http += req.substr(0, i);
	req.erase(0, i);
	i = 0;
	if (req[i] == '\r')
	{
		req.erase(0, 2);
		return;
	}
	else if (req[i] != '.' || !(req[i + 1] >= '0' && req[i + 1] <= '9'))
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	http += ".";
	req.erase(0, 1);
	while (req[i] && (req[i] >= '0' && req[i] <= '9'))
		i++;
	n = std::atoi(req.c_str());
	if (n < 0 || n > std::numeric_limits<int>::max())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	http += req.substr(0, i);
	req.erase(0, i);
	if (req[0] != '\r' && req[1] != '\n')
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HTTP");
	}
	req.erase(0, 2);
	this->_request_http_version = http;
}

void	RequestParser::parseHeaders(std::string &req)
{
	std::string::iterator	it;
	std::string::iterator	its;
	std::string				temp_key;
	std::string				temp_value;
	size_t					pos = 0;
	size_t					end = 0;

	if (req.find("\r\n\r\n") == std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("INVALID HEADERS END");
	}
	while (!req.empty())
	{
		end = req.find("\r\n");
		if (end == 0)
			break;
		pos = req.find_first_of(":");
		if (pos == std::string::npos || pos > end)
		{
			if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(400);
			throw RequestParser::RequestException("INVALID HEADERS KEY");
		}
		temp_key = req.substr(0, pos);
		it = std::find_if(temp_key.begin(), temp_key.end(), is_non_printable);
		its = std::find_if(temp_key.begin(), temp_key.end(), is_space);
		if (it != temp_key.end() || its != temp_key.end())
		{
			if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(400);
			throw RequestParser::RequestException("INVALID HEADERS KEY");
		}
		if (req.find_first_not_of(" \t", pos + 1) == pos + 1)
			temp_value = req.substr(pos + 1, end - pos - 1);
		else if (req.find_first_not_of(" \t", pos + 1) == end)
			temp_value = "";
		else
		{
			pos = req.find_first_not_of(" \t", pos + 1);
			temp_value = req.substr(pos, end - pos);
		}
		its = std::find_if(temp_value.begin(), temp_value.end(), is_value_valid);
		if (its != temp_value.end())
		{
			if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(400);
			throw RequestParser::RequestException("INVALID HEADERS VALUE");
		}
		this->_request_headers.insert(std::make_pair(temp_key, temp_value));
		req.erase(0, end + 2);
	}
	req.erase(0, 2);
}

void	RequestParser::validateHeaders(void)
{
	std::map<std::string, std::string>::iterator	it;
	std::string			cntt_type;
	bool				host = 0;

	for (it = this->_request_headers.begin(); it != this->_request_headers.end(); it++)
	{
		if (it->first == "Host")
			host = 1;
		else if (it->first == "Content-Length")
		{
			this->_cnt_lenght = 1;
			if (it->second.find_first_not_of("0123456789") != std::string::npos)
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(400);
				throw RequestParser::RequestException("INVALID HEADER Content-Length");
			}
			this->_content_length = std::strtoul(it->second.c_str(), NULL, 0);
		}
		else if (it->first == "Transfer-Encoding")
		{
			this->_transfert_encoding = 1;
			if (it->second != "chunked")
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(400);
				throw RequestParser::RequestException("Transfer-encoding error");
			}
		}
		else if (it->first == "Content-Type")
		{
			this->_content_type = it->second;
		}
		else if (it->first == "Cookie")
		{
			this->_cookie = it->second;
		}
		else if (it->first == "Connection")
		{
			this->_connection = it->second;
		}
	}
	if (host == 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("HOST HEADER IS MISSING");
	}
	if (this->_request_method == "POST" && this->_cnt_lenght == 0 && this->_transfert_encoding == 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("BODY LENTGHIS MISSING");
	}
}


void	RequestParser::parseBody(std::string &req, int clientFd)
{
	std::cout << "\033[32mParsing Request Body\033[0m" << std::endl << std::endl;

	
	std::string	buf = req;
	std::string	temp_buf;
	std::string	str_chunk_size;
	ssize_t		bytes_received;
	size_t		pos;
	long		chunk_size;

	if (this->_cnt_lenght == 1 && this->_transfert_encoding == 0)
	{
		// std::cout << "Content length = " << this->_content_length << std::endl;
		// std::cout << "req.size() = " << req.size() << std::endl;
		// std::cout << req << std::endl;
		while (req.size() < this->_content_length)
		{
			temp_buf.resize(BUFFER_SIZE);
			bytes_received = recv(clientFd, &temp_buf[0], BUFFER_SIZE, 0);
			if (bytes_received <= 0)
			{
				std::cout << "body size: " << req.size() << std::endl;
				if (bytes_received < 0)
				{
					if (this->_error_class->getErrorCode() == 0)
						this->_error_class->setErrorCode(500);
					throw RequestParser::RequestException("ERROR FROM RECV()");
				}
				if (bytes_received == 0 && req.size() < this->_content_length)
				{
					if (this->_error_class->getErrorCode() == 0)
						this->_error_class->setErrorCode(400);
					throw RequestParser::RequestException("ERROR WITH BODY SIZE");
				}
			}
			req += temp_buf.substr(0, bytes_received);
		}
		this->_request_body = req;
		return ;
	}
	if (this->_transfert_encoding == 1)
	{
		std::cout << "Transfer encoding body" << std::endl;
		while (true)
		{
			pos = buf.find("\r\n");
			if (pos == std::string::npos)
			{
				temp_buf.resize(BUFFER_SIZE);
				bytes_received = recv(clientFd, &temp_buf[0], BUFFER_SIZE, 0);
				if (bytes_received < 0 || (bytes_received == 0 && temp_buf.find("0\r\n\r\n") == std::string::npos))
				{
					// std::cout << "buf if byte received <= 0: *" << buf << "*" << std::endl;
					// std::cout << "temp_buf if byte received <= 0: *" << temp_buf << "*" << std::endl;
					if (this->_error_class->getErrorCode() == 0)
						this->_error_class->setErrorCode(400);
					throw RequestParser::RequestException("ERROR WITH BODY");
				}
				buf += temp_buf;
				temp_buf.clear();
			}
			else
			{
				str_chunk_size = buf.substr(0, pos);
				// std::cout << "*" << str_chunk_size << "*" << std::endl;
				chunk_size = extract_size(str_chunk_size);
				// std::cout << "chunk size : " << chunk_size << std::endl;
				if (chunk_size == -1)
				{
					if (this->_error_class->getErrorCode() == 0)
						this->_error_class->setErrorCode(400);
					throw RequestParser::RequestException("ERROR WITH BODY CHUNK SIZE");
				}
				else if (chunk_size == 0)
				{
					temp_buf.resize(BUFFER_SIZE);
					while ((bytes_received = recv(clientFd, &temp_buf[0], BUFFER_SIZE, 0)) > 0)
						buf += temp_buf;
					if (bytes_received < 0)
					{
						if (this->_error_class->getErrorCode() == 0)
							this->_error_class->setErrorCode(500);
						throw RequestParser::RequestException("Error from recv() chunked");
					}
					if (buf.find("0\r\n\r\n") != 0)
					{
						if (this->_error_class->getErrorCode() == 0)
							this->_error_class->setErrorCode(400);
						throw RequestParser::RequestException("ERROR WITH BODY END");
					}
					return ;
				}
				buf.erase(0, pos + 2);
				while (buf.size() < static_cast<size_t> (chunk_size + 2))
				{
					temp_buf.resize(BUFFER_SIZE);
					bytes_received = recv(clientFd, &temp_buf[0], BUFFER_SIZE, 0);
					//std::cerr << bytes_received << std::endl;
					if (bytes_received <= 0)
					{
						if (this->_error_class->getErrorCode() == 0)
							this->_error_class->setErrorCode(400);
						throw RequestParser::RequestException("ERROR WITH BODY");
					}
					temp_buf.resize(bytes_received);
					buf += temp_buf;
				}
				// std::cout << buf.substr(0, chunk_size) << "*" << std::endl << std::endl;
				this->_request_body += buf.substr(0, chunk_size);
				// std::cout << "Request body:" << std::endl;
				// std::cout << this->_request_body << std::endl;
				buf.erase(0, chunk_size + 2);
				// std::cout << std::endl << std::endl;
			}
		}
	}
}
