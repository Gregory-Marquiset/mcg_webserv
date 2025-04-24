/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:01:57 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/24 07:49:58 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request/ProcessRequest.hpp"
#include "../../includes/utils/Utils.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
ProcessRequest::ProcessRequest(void)
{
}

ProcessRequest::ProcessRequest(Server *serv, RequestParser &req, ErrorManagement &err) : _serv_info(serv), _request(req), _error_class(&err),
_method(req.getMethod()), _http_version(req.getHTTP()), _request_body(req.getBody()), _headers(req.getHeaders()), _cgi(false),
_autoindex(false), _index(true)
{
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
		this->_content_type = inst._content_type;
		this->_cgi_path = inst._cgi_path;
		this->_headers = inst._headers;
		this->_cgi = inst._cgi;
		this->_autoindex = inst._autoindex;
		this->_index = inst._index;
	}
	return (*this);
}


/* ================= SETTERS ======================== */
void	ProcessRequest::setIsCgi(bool value)
{
	this->_cgi = value;
}

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

std::string	ProcessRequest::getContentType(void) const
{
	return (this->_content_type);
}

std::map<std::string, std::string>	ProcessRequest::getHeaders(void) const
{
	return (this->_headers);
}

std::string	ProcessRequest::getCgiPath(void) const
{
	return (this->_cgi_path);
}

bool	ProcessRequest::getCgi(void) const
{
	return (this->_cgi);
}

bool	ProcessRequest::getAutoIndex(void) const
{
	return (this->_autoindex);
}

bool	ProcessRequest::getIndex(void) const
{
	return (this->_index);
}

/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */
void	ProcessRequest::processRequest(void)
{
	std::cout << std::endl << "PROCESS DE LA REQUETE" << std::endl;
	try
	{
		this->compareUriWithLocations();
		this->checkAllowedMethod();
		this->checkMaxBodySize();
		this->addRootPath();
		this->checkIfUriIsCgi();
		std::cout << "cgi status: " << this->_cgi << std::endl;
		if (this->_cgi == true)
		{
			return;
		}
		if (this->_method == "POST")
		{
			this->processPostRequest();
		}
	}
	catch (RequestParser::RequestException	&req_exc)
	{
		std::cerr << req_exc.what() << std::endl;
	}
}


/* ================= PRIVATE MEMBER FUNCTIONS ======================== */

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
		// std::cout << "location path: " << location_path << std::endl;
		// if (it->getCgiExtension().empty())
		// {
		// 	std::cout << "cgi class is empty" << std::endl;
		// }
		// else
		// 	std::cout << "not empty" << std::endl;
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

	// this->_cgi = true;
	// this->_cgi_path = "/usr/bin/python3";
	// return;
	
	cgi_ext = this->_location_to_use.getCgiExtension();
	if (cgi_ext.empty())
	{
		std::cout << "prout" << std::endl;
		return ;
		// if (this->_error_class->getErrorCode() == 0)
		// 	this->_error_class->setErrorCode(500);
		// throw RequestParser::RequestException("Block CGI manquant");
	}
	uri = this->_final_path;
	pos = uri.rfind(".");
	if (pos == std::string::npos)
		return ;
	extension = uri.substr(pos, std::string::npos);
	//std::cout << "extension: " << extension << std::endl;
	for (std::vector<CgiHandler>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
	{
		//std::cout << "extension: " << extension << " et it->getKey" << it->getKey() << std::endl;
		if (extension == it->getKey())
		{
			this->setIsCgi(true);
			this->_cgi_path = it->getValue();
			std::cout << "cgi_path: " << this->_cgi_path << std::endl;
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
	if (!redir.empty() && this->_request.getMethod() == "GET")
	{
		// std::cout << "redir[0] = " << redir[0] << std::endl;
		// std::cout << "redir[1] = " << redir[1] << std::endl;
		if (this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(Utils::strtoi(redir[0]));
		this->_final_path = redir[1];
		// std::cout << this->_error_class->getErrorCode() << std::endl;
		// std::cout << this->_final_path << std::endl;
		return;
	}

	std::cout << "Root path: " << this->_location_to_use.getRoot() << std::endl;
	this->_final_path += this->_location_to_use.getRoot();
	path_size = this->_final_path.size();
	if (this->_final_path[path_size - 1] != '/')
		this->_final_path += "/";

	uri = this->_request.getURI();
	uri.erase(0, this->_location_to_use.getPath().size());
	this->_final_path += uri;

	path_size = this->_final_path.size();
	if (this->_final_path[path_size - 1] == '/' && this->_request.getMethod() != "POST")
	{
		std::string	index = this->_location_to_use.getIndex();

		if (index.empty())
		{
			_index = false;
			if (this->_location_to_use.getAutoIndex() == "on")
			{
				if (this->getMethod() == "GET")
					return (this->extractDirectoryContent());
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
		if (it->second == "dir")
			body += "        <p><a href=\"" + it->first + "/" + "\">" + it->first + "/</a></p>\n";
		else
			body += "        <p><a href=\"" + it->first + "\">" + it->first + "</a></p>\n";
	}
	body += "    </div>\n";
	body += "</body>\n";
	body += "</html>\n";

	this->_request_body = body;
}

void	ProcessRequest::processPostRequest(void)
{
	std::string	path = this->_final_path;

	std::cout << "path dans process post : " << path << std::endl;
	if (access(path.c_str(), F_OK) == 0 && opendir(path.c_str()) == NULL)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(409);
		throw RequestParser::RequestException("Fichier déjà existant");
	}
	if (opendir("./www/upload") == NULL)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(500);
		throw RequestParser::RequestException("Dossier \"uploads\" inexistant");
	}
	if (this->_request.getContentType() == "application/x-www-form-urlencoded")
		this->manageFormCase();
	else if (this->_request.getContentType().find("multipart/form-data") != std::string::npos)
		this->manageMultipartCase();
	else if (this->_request.getContentType() == "application/json")
		this->manageJsonCase();
	else if (this->_request.getContentType() == "text/plain")
		this->manageSimpleCase();
	else
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("Bad Content Type");
	}
}

void	ProcessRequest::manageFormCase(void)
{
	std::cout << "CAS POST form" << std::endl;
	std::string	path = this->_final_path;
	std::string	new_path;
	bool		generate = false;
	size_t		slash_pos = 0;
	size_t		dot_pos = std::string::npos;
	size_t		size;

	if (path.find("..") != std::string::npos || path.find("\\") != std::string::npos || path.find(":") != std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Erreur dans le path pour POST");
	}
	if (path.find("./www/upload/") == 0)
		path.erase(0, 13);
	else if (path.find("./www/") == 0)
		path.erase(0, 6);

	if (!path.empty())
	{
		slash_pos = path.rfind("/");
		if (slash_pos == std::string::npos)
			slash_pos = 0;
		dot_pos = path.find(".", slash_pos);
	}
	if (dot_pos == std::string::npos)
		generate = true;
	else if (Utils::authorizedMIME(path.substr(dot_pos)) != 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Extension interdite pour POST");
	}

	if (!path.empty())
	{
		size = path.size();
		for (size_t i = 0; i < size; i++)
		{
			if (path[i] == '/')
				path[i] = '_';
		}
	}
	if (generate == true)
	{
		size = path.size();
		if (!path.empty() && path[size] != '_')
			path += "_";
		path += Utils::getTime(1) + ".txt";
	}
	new_path += "./www/upload/" + path;

	if (access(new_path.c_str(), F_OK) == 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(409);
		throw RequestParser::RequestException("Fichier déja existant");
	}
	std::ofstream	file(new_path.c_str());

	if (!file.is_open())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(500);
		throw RequestParser::RequestException("Fichier non crée");
	}
	std::string	body = this->getBody();

	size = body.size();
	for (size_t i = 0; i < size; i++)
	{
		if (body[i] == '&')
			body[i] = '\n';
	}
	file << body;
}

void	ProcessRequest::manageMultipartCase(void)
{
	std::cout << "CAS POST multipart" << std::endl;
	
	std::string	request_content_type = this->_request.getContentType();
	std::string	boundary;
	size_t		pos = 0;

	pos = request_content_type.find("boundary=");
	if (pos == std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(400);
		throw RequestParser::RequestException("Boundary absent");
	}
	boundary = request_content_type.substr(pos + 9);
	std::cout << "Boundary: " << boundary << std::endl;
	
	std::string					body = this->_request.getBody();
	size_t						start;
	size_t						end;
	size_t						token_start;
	size_t						token_end;
	
	// int n = 0;
	// for (size_t i = 0; i < body.size(); i++)
	// {
	// 	if (body[i] == '\r' && body[i + 1] == '\n')
	// 	{
	// 		n++;
	// 		std::cerr << "prout " << n << "\ti = " << i << std::endl;
	// 	}
	// }
	// std::cerr << "n = " << n << std::endl;
	
	end = body.find(boundary);
	while (1)
	{
		start = end;
		if (start == std::string::npos)
		{
			if(this->_error_class->getErrorCode() == 0)
				this->_error_class->setErrorCode(400);
			throw RequestParser::RequestException("Boundary error");
		}
		end = body.find(boundary, boundary.size());
		// std::cout << "start: " << start << std::endl;
		// std::cout << "end: " << end << std::endl;
		// std::cout << "npos: " << std::string::npos << std::endl << std::endl;
		if (end == std::string::npos)
		{
			std::cout << "the end" << std::endl;
			break;
		}
		std::string	type;
		std::string	filename = "./www/upload/";
		std::string	file_body;
		
		token_start = body.find("name=\"");
		token_end = body.find("\"", token_start + 6);
		type = body.substr(token_start + 6, token_end - token_start - 6);
		body.erase(0, token_end - 1);
		// std::cout << "token_start: " << start << std::endl;
		// std::cout << "token_end: " << end << std::endl;
		// std::cout << "type: " << type << std::endl << std::endl;
		if (type == "file" || type == "files[]")
		{
			token_start = body.find("filename=\"");
			token_end = body.find("\"", token_start + 10);
			filename += body.substr(token_start + 10, token_end - token_start - 10);
			std::cout << "token_start: " << start << std::endl;
			std::cout << "token_end: " << end << std::endl;
			std::cout << "filename: " << filename << std::endl;
			
			size_t	pos = body.find("\r\n\r\n");
			if (pos == std::string::npos)
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(400);
				throw RequestParser::RequestException("Pas de double retour a la ligne");
			}
			body.erase(0, pos + 4);
			file_body += body.substr(0, end);
			
			if (access(filename.c_str(), F_OK) == 0)
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(409);
				throw RequestParser::RequestException("Fichier déja existant");
			}
			std::ofstream	file(filename.c_str());
		
			if (!file.is_open())
			{
				if (this->_error_class->getErrorCode() == 0)
					this->_error_class->setErrorCode(500);
				throw RequestParser::RequestException("Fichier non crée");
			}
			std::string	body = this->getBody();
		
			file << file_body;
		}
	}
}

void	ProcessRequest::manageJsonCase(void)
{
	std::cout << "CAS POST json" << std::endl;
	std::string	path = this->_final_path;
	std::string	new_path;
	bool		generate = false;
	size_t		slash_pos = 0;
	size_t		dot_pos = std::string::npos;
	size_t		size;

	if (path.find("..") != std::string::npos || path.find("\\") != std::string::npos || path.find(":") != std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Erreur dans le path pour POST");
	}
	if (path.find("./www/upload/") == 0)
		path.erase(0, 13);
	else if (path.find("./www/") == 0)
		path.erase(0, 6);

	if (!path.empty())
	{
		slash_pos = path.rfind("/");
		if (slash_pos == std::string::npos)
			slash_pos = 0;
		dot_pos = path.find(".", slash_pos);
	}
	if (dot_pos == std::string::npos)
		generate = true;
	else if (path.substr(dot_pos) != ".json")
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Extension n'est pas .json");
	}

	if (!path.empty())
	{
		size = path.size();
		for (size_t i = 0; i < size; i++)
		{
			if (path[i] == '/')
				path[i] = '_';
		}
	}
	if (generate == true)
	{
		size = path.size();
		if (!path.empty() && path[size] != '_')
			path += "_";
		path += Utils::getTime(1) + ".json";
	}
	new_path += "./www/upload/" + path;

	if (access(new_path.c_str(), F_OK) == 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(409);
		throw RequestParser::RequestException("Fichier déja existant");
	}
	std::ofstream	file(new_path.c_str());

	if (!file.is_open())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(500);
		throw RequestParser::RequestException("Fichier non crée");
	}
	std::string	body = this->getBody();

	file << body;
}

void	ProcessRequest::manageSimpleCase(void)
{
	std::cout << "CAS POST text/plain" << std::endl;

	std::string	path = this->_final_path;
	std::string	new_path;
	bool		generate = false;
	size_t		slash_pos = 0;
	size_t		dot_pos = std::string::npos;
	size_t		size;

	if (path.find("..") != std::string::npos || path.find("\\") != std::string::npos || path.find(":") != std::string::npos)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Erreur dans le path pour POST");
	}
	if (path.find("./www/upload/") == 0)
		path.erase(0, 13);
	else if (path.find("./www/") == 0)
		path.erase(0, 6);

	if (!path.empty())
	{
		slash_pos = path.rfind("/");
		if (slash_pos == std::string::npos)
			slash_pos = 0;
		dot_pos = path.find(".", slash_pos);
	}
	if (dot_pos == std::string::npos)
		generate = true;
	else if (path.substr(dot_pos) != ".txt")
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(403);
		throw RequestParser::RequestException("Extension n'est pas .txt");
	}

	if (!path.empty())
	{
		size = path.size();
		for (size_t i = 0; i < size; i++)
		{
			if (path[i] == '/')
				path[i] = '_';
		}
	}
	if (generate == true)
	{
		size = path.size();
		if (!path.empty() && path[size] != '_')
			path += "_";
		path += Utils::getTime(1) + ".txt";
	}
	new_path += "./www/upload/" + path;
	std::cout << "new_path : " << new_path << std::endl;

	if (access(new_path.c_str(), F_OK) == 0)
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(409);
		throw RequestParser::RequestException("Fichier déja existant");
	}
	std::ofstream	file(new_path.c_str());

	if (!file.is_open())
	{
		if (this->_error_class->getErrorCode() == 0)
			this->_error_class->setErrorCode(500);
		throw RequestParser::RequestException("Fichier non crée");
	}
	std::string	body = this->getBody();

	file << body;
}
