/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:01:57 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/11 08:57:19 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/utils/Utils.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ProcessRequest::ProcessRequest(void)
{
}

ProcessRequest::ProcessRequest(Server *serv, RequestParser &req, ErrorManagement &err) : _serv_info(serv), _request(req), _error_class(&err),
_method(req.getMethod()), _http_version(req.getHTTP()), _request_body(req.getBody()), _headers(req.getHeaders()), _cgi(req.getIsCgi()), _autoindex(false)
{
	this->processRequest();
}

ProcessRequest::ProcessRequest(ProcessRequest const &copy)
{
	*this = copy;
}

ProcessRequest::~ProcessRequest(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
ProcessRequest	&ProcessRequest::operator=(ProcessRequest const &inst)
{
	if (this != &inst)
	{
		this->_serv_info = inst._serv_info;
		this->_request = inst._request;
		this->_error_class = inst._error_class;
		this->_location_to_use = inst._location_to_use;
		this->_final_path = inst._final_path;
		this->_method = inst._method;
		this->_http_version = inst._http_version;
		this->_request_body = inst._request_body;
		this->_headers = inst._headers;
		this->_cgi = inst._cgi;
		this->_autoindex = inst._autoindex;
	}
	return (*this);
}


/* ================= SETTERS ======================== */


/* ================= GETTERS ======================== */
std::string	ProcessRequest::getFinalPath(void) const
{
	return (this->_final_path);
}

std::string	ProcessRequest::getMethod(void) const
{
	return (this->_method);
}

std::string	ProcessRequest::getHTTP(void) const
{
	return (this->_http_version);
}

std::string	ProcessRequest::getBody(void) const
{
	return (this->_request_body);
}

std::map<std::string, std::string>	ProcessRequest::getHeaders(void) const
{
	return (this->_headers);
}

bool	ProcessRequest::getCgi(void) const
{
	return (this->_cgi);
}

bool	ProcessRequest::getAutoIndex(void) const
{
	return (this->_autoindex);
}

/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */
void	ProcessRequest::processRequest(void)
{
	try
	{
		this->compareUriWithLocations();
		this->checkAllowedMethod();
		this->checkMaxBodySize();
		this->addRootPath();
		this->checkIfUriIsCgi();
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << req_exc.what() << std::endl;
	}
}

void	ProcessRequest::compareUriWithLocations(void)
{
	std::vector<LocationBlock>::iterator	it;
	std::vector<LocationBlock>				locations;
	std::string								uri;
	std::string								location_path;
	int										uri_size;
	int										location_path_size;
	int										last_location_match_size = -1;

	uri = this->_request.getURI();
	uri_size = uri.size();
	locations = this->_serv_info->getServerBlock().getLocationBlock();

	for (it = locations.begin(); it != locations.end(); it++)
	{
		location_path = it->getPath();
		if (location_path == "/" && last_location_match_size == -1)
			this->_location_to_use = *it;
		location_path_size = location_path.size();
		if (uri.compare(0, location_path_size, location_path) == 0)
		{
			if (uri_size == location_path_size || uri[location_path_size] == '/')
			{
				if (location_path_size > last_location_match_size)
				{
					last_location_match_size = location_path_size;
					this->_location_to_use = *it;
				}
			}
		}
	}
}

void	ProcessRequest::checkAllowedMethod(void)
{
	std::vector<std::string>	methods;

	methods = this->_location_to_use.getAllowMethods();
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (this->_request.getMethod() == *it)
			return ;
	}
	if (this->_error_class->getErrorCode() == 0)
		this->_error_class->setErrorCode(405);
	throw RequestParser::RequestException("Method not allowed in this location");
}

void	ProcessRequest::checkMaxBodySize(void)
{
	std::string	str_max_body_size = this->_location_to_use.getClientMaxBodySize();
	size_t		pos;
	size_t		multiplier = 0;
	size_t		max_body_size = 0;

	//std::cout << "str max body: " << str_max_body_size << std::endl;
	if (str_max_body_size.empty())
		str_max_body_size += "1M";
	pos = str_max_body_size.find_first_not_of("0123456789");

	if (pos != std::string::npos)
	{
		if ((str_max_body_size[pos] == 'k' || str_max_body_size[pos] == 'K') && pos == str_max_body_size.size() - 1)
		{
			multiplier += 1000;
		}
		else if ((str_max_body_size[pos] == 'm' || str_max_body_size[pos] == 'M') && pos == str_max_body_size.size() - 1)
		{
			multiplier += 1000000;
		}
		else
		{
			if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(500);
			throw RequestParser::RequestException("Error with Max Body size in config");
		}
	}
	std::istringstream	iss(str_max_body_size);
	iss >> max_body_size;
	//std::cout << "max body size before mult: " << max_body_size << std::endl;
	if (multiplier != 0)
		max_body_size *= multiplier;
	//std::cout << "max body size after mult: " << max_body_size << std::endl;
	if (this->_request.getBody().size() > max_body_size)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(413);
		throw RequestParser::RequestException("Body size is bigger than client max body size");
	}
}

void	ProcessRequest::checkIfUriIsCgi(void)
{
	std::vector<CgiHandler>	cgi_ext;
	std::string				uri;
	std::string				extension;
	size_t					pos;

	cgi_ext = this->_location_to_use.getCgiExtension();
	uri = this->_request.getURI();
	pos = uri.rfind(".");
	if (pos == std::string::npos)
		return ;
	extension = uri.substr(pos, std::string::npos);
	for (std::vector<CgiHandler>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
	{
		if (extension == it->getKey())
		{
			this->_request.setIsCgi(true);
			return ;
		}
	}
}

void	ProcessRequest::addRootPath(void)
{
	std::vector<std::string>	redir;
	std::string	uri;
	size_t		path_size;

	std::cout << "Uri sans modif : " << this->_request.getURI() << std::endl;

	redir = this->_location_to_use.getRedirection();
	if (!redir.empty())
	{
		std::cout << "redir[0] = " << redir[0] << std::endl;
		std::cout << "redir[1] = " << redir[1] << std::endl;
		if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(Utils::strtoi(redir[0]));
		this->_final_path = redir[1];
		std::cout << this->_error_class->getErrorCode() << std::endl;
		std::cout << this->_final_path << std::endl;
		return;
	}

	this->_final_path += this->_location_to_use.getRoot();
	path_size = this->_final_path.size();
	if (this->_final_path[path_size - 1] != '/')
		this->_final_path += "/";

	uri = this->_request.getURI();
	uri.erase(0, this->_location_to_use.getPath().size());
	this->_final_path += uri;

	path_size = this->_final_path.size();
	if (this->_final_path[path_size - 1] == '/')
	{
		std::string	index = this->_location_to_use.getIndex();

		if (index.empty())
		{
			if (this->_location_to_use.getAutoIndex() == "on")
			{
				if (this->getMethod() == "GET")
				{
					std::cout << "prout" << std::endl;
					return (this->extractDirectoryContent());
				}
			}
			if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(403);
			throw RequestParser::RequestException("Index is empty");
		}
		this->_final_path += index;
	}
	std::cout << "final path: " << this->_final_path << std::endl;
}

void	ProcessRequest::extractDirectoryContent(void)
{
	std::cout << "Ca passe dans extract directory content" << std::endl;
	DIR	*directory = opendir(this->getFinalPath().c_str());

	if (directory == NULL)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Can't open directory");
	}

	std::map<std::string, std::string>	directory_content;
	struct dirent	*dir = readdir(directory);
	while (dir != NULL)
	{
		std::string	name(dir->d_name);
		std::string	type;

		if (dir->d_type == DT_DIR)
			type = "dir";
		else
			type = "file";

		directory_content[name] = type;
		dir = readdir(directory);
	}
	closedir(directory);
	this->generateHTMLBody(directory_content);
	this->_autoindex = true;
}

void	ProcessRequest::generateHTMLBody(std::map<std::string, std::string>	&directory_content)
{
	std::string	body;

	body += "<!DOCTYPE html>\n";
	body += "<html lang=\"en\">\n";
	body += "<head>\n";
	body += "    <meta charset=\"UTF-8\">\n";
	body += "    <title>Index of /www/</title>\n";
	body += "    <style>\n";
	body += "        body { font-family: Arial, sans-serif; padding: 20px; }\n";
	body += "        h1 { font-size: 24px; }\n";
	body += "        a { text-decoration: none; color: #007BFF; }\n";
	body += "        a:hover { text-decoration: underline; }\n";
	body += "        .file-list { margin-top: 20px; }\n";
	body += "    </style>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "    <h1>Index of /www</h1>\n";
	body += "    <div class=\"file-list\">\n";

	for (std::map<std::string, std::string>::iterator it = directory_content.begin(); it != directory_content.end(); it++)
	{
		// if (it->second == "dir")
		// 	body += "        <p><a href=\"" + it->first + "\">" + it->first + "/</a></p>\n";
		// else
		body += "        <p><a href=\"" + it->first + "\">" + it->first + "</a></p>\n";
	}
	body += "    </div>\n";
	body += "</body>\n";
	body += "</html>\n";

	this->_request_body = body;
}
