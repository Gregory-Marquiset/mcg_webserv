
#ifndef CGI_HPP
# define CGI_HPP

# include "../WebServ.hpp"

std::string we_checkCGI(const std::string &binary, const std::string &file, const std::string &cookieHeader, const std::string &body, ErrorManagement &err);

#endif
