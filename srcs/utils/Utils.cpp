/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdutel <cdutel@42student.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:06:48 by cdutel            #+#    #+#             */
/*   Updated: 2025/04/10 13:46:56 by cdutel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/utils/Utils.hpp"

/* ================= CONSTRUCTEUR - DESTRUCTEUR ======================== */
Utils::Utils(void)
{
}

Utils::Utils(Utils const &copy)
{
	*this = copy;
}

Utils::~Utils(void)
{
}


/* ================= OPERATOR OVERLOAD ======================== */
Utils	&Utils::operator=(Utils const &inst)
{
	if (this != &inst)
	{

	}
	return (*this);
}


/* ================= SETTERS ======================== */


/* ================= GETTERS ======================== */


/* ================= NON MEMBER FUNCTIONS ======================== */


/* ================= PUBLIC MEMBER FUNCTIONS ======================== */
std::string	Utils::getTime(void)
{
	char		buf[80];
	time_t		rawTime;
	struct tm	*timeInfo;

	std::time(&rawTime);
	timeInfo = std::gmtime(&rawTime);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", timeInfo);

	return (std::string(buf));
}

std::string	Utils::findMIME(std::string path)
{
	std::map<std::string, std::string>	mime_type;
	std::string							extension;
	size_t								pos;

	mime_type[".aac"] = "audio/aac";
	mime_type[".abw"] = "application/x-abiword";
	mime_type[".arc"] = "application/octet-stream";
	mime_type[".avi"] = "video/x-msvideo";
	mime_type[".azw"] = "application/vnd.amazon.ebook";
	mime_type[".bin"] = "application/octet-stream";
	mime_type[".bmp"] = "image/bmp";
	mime_type[".bz"] = "application/x-bzip";
	mime_type[".bz2"] = "application/x-bzip2";
	mime_type[".csh"] = "application/x-csh";
	mime_type[".css"] = "text/css";
	mime_type[".csv"] = "text/csv";
	mime_type[".doc"] = "application/msword";
	mime_type[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_type[".eot"] = "application/vnd.ms-fontobject";
	mime_type[".epub"] = "application/epub+zip";
	mime_type[".gif"] = "image/gif";
	mime_type[".htm"] = "text/html";
	mime_type[".html"] = "text/html";
	mime_type[".ico"] = "image/x-icon";
	mime_type[".ics"] = "text/calendar";
	mime_type[".jar"] = "application/java-archive";
	mime_type[".jpeg"] = "image/jpeg";
	mime_type[".jpg"] = "image/jpeg";
	mime_type[".js"] = "application/javascript";
	mime_type[".json"] = "application/json";
	mime_type[".mid"] = "audio/midi";
	mime_type[".midi"] = "audio/midi";
	mime_type[".mpeg"] = "video/mpeg";
	mime_type[".mpkg"] = "application/vnd.apple.installer+xml";
	mime_type[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mime_type[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mime_type[".odt"] = "application/vnd.oasis.opendocument.text";
	mime_type[".oga"] = "audio/ogg";
	mime_type[".ogv"] = "video/ogg";
	mime_type[".ogx"] = "application/ogg";
	mime_type[".otf"] = "font/otf";
	mime_type[".png"] = "image/png";
	mime_type[".pdf"] = "application/pdf";
	mime_type[".ppt"] = "application/vnd.ms-powerpoint";
	mime_type[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_type[".rar"] = "application/x-rar-compressed";
	mime_type[".rtf"] = "application/rtf";
	mime_type[".sh"] = "application/x-sh";
	mime_type[".svg"] = "image/svg+xml";
	mime_type[".swf"] = "application/x-shockwave-flash";
	mime_type[".tar"] = "application/x-tar";
	mime_type[".tif"] = "image/tiff";
	mime_type[".tiff"] = "image/tiff";
	mime_type[".ts"] = "application/typescript";
	mime_type[".ttf"] = "font/ttf";
	mime_type[".vsd"] = "application/vnd.visio";
	mime_type[".wav"] = "audio/x-wav";
	mime_type[".weba"] = "audio/webm";
	mime_type[".webm"] = "audio/webm";
	mime_type[".webp"] = "image/webp";
	mime_type[".woff"] = "font/woff";
	mime_type[".woff2"] = "font/woff2";
	mime_type[".xhtml"] = "application/xhtml+xml";
	mime_type[".xls"] = "application/vnd.ms-excel";
	mime_type[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_type[".xml"] = "application/xml";
	mime_type[".xul"] = "application/vnd.mozilla.xul+xml";
	mime_type[".zip"] = "application/zip";
	mime_type[".7z"] = "application/x-7z-compressed";

	pos = path.rfind(".");
	if (pos != std::string::npos)
	{
		extension = path.substr(pos);
		for (std::map<std::string, std::string>::iterator it = mime_type.begin(); it != mime_type.end(); it++)
		{
			if (extension == it->first)
				return (it->second);
		}
	}
	return ("application/octet-stream");
}

std::map<int, std::string>	Utils::get_error_map(void)
{
	std::map<int, std::string>	error_map;

	error_map[400] = "./www/error/4XX/400.html";
	error_map[403] = "./www/error/4XX/403.html";
	error_map[404] = "./www/error/4XX/404.html";
	error_map[405] = "./www/error/4XX/405.html";
	error_map[408] = "./www/error/4XX/408.html";
	error_map[413] = "./www/error/4XX/413.html";
	error_map[414] = "./www/error/4XX/414.html";
	error_map[415] = "./www/error/4XX/415.html";
	error_map[500] = "./www/error/5XX/500.html";
	error_map[501] = "./www/error/5XX/501.html";
	error_map[502] = "./www/error/5XX/502.html";
	error_map[503] = "./www/error/5XX/503.html";
	error_map[504] = "./www/error/5XX/504.html";

	return (error_map);
}

std::string	Utils::getErrorString(int error_code)
{
	std::map<int, std::string>	error_map;

	error_map[301] = "301 Moved Permanently";
	error_map[302] = "302 Found";
	error_map[400] = "400 Bad Request";
	error_map[403] = "403 Forbidden";
	error_map[404] = "404 Not Found";
	error_map[405] = "405 Method Not Allowed";
	error_map[408] = "408 Request Timeout";
	error_map[413] = "413 Request Entity Too Large";
	error_map[414] = "414 Request-URI Too Long";
	error_map[415] = "415 Unsupported Media Type";
	error_map[500] = "500 Internal Server Error";
	error_map[501] = "501 Not Implemented";
	error_map[502] = "502 Bad Gateway";
	error_map[503] = "503 Service Unavailable";
	error_map[504] = "504 Gateway Timeout";

	for (std::map<int, std::string>::iterator it = error_map.begin(); it != error_map.end(); it++)
	{
		if (error_code == it->first)
			return (it->second);
	}
	return ("500 Internal Server Error");
}

int	Utils::strtoi(std::string str)
{
	if (str.empty())
		return (-1);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isxdigit(str[i]))
			return (-1);
	}
	std::istringstream	iss(str);
	int					n;

	iss >> n;
	//std::cout << "chunk_size in extract_size : " << chunk_size << std::endl;
	return (n);
}
