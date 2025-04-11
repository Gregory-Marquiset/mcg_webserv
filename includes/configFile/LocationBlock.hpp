#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include "../WebServ.hpp"
#include "Block.hpp"
#include "RecupBlockContent.hpp"
#include "CgiHandler.hpp"

class LocationBlock {

    private:
       std::string _path;
       std::string _root;
       std::string _index;
       std::vector<std::string> _allowMethods;
       std::vector<CgiHandler> _cgiExtension;
       std::string _client_max_body_size;
       std::string _auto_index;
       std::vector<std::string> _redirection;

    public:
        LocationBlock();
        ~LocationBlock();

        /* setters */

        void setPath(std::string path);
        void setPathSpecial(std::string path);
        void setRoot(std::string root);
        void setIndex(std::string index);
        void setAllowMethods(std::vector<std::string> allowMethods);
        void setCgiExtension(std::vector<CgiHandler> cgiExtension);
        void setClientMaxBodySize(std::string client_max_body_size);
        void setAutoIndex(std::string status);
        void setRedirection(std::vector<std::string> redirection);
        
        /* getters */

        std::string getPath() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::vector<std::string> getAllowMethods() const;
        std::vector<CgiHandler> getCgiExtension() const;
        std::string getClientMaxBodySize() const;
        std::string getAutoIndex() const;
        std::vector<std::string> getRedirection() const;

        void addCgi(const CgiHandler& cgi);
        void addAllowMethod(const std::string& method);

};

#endif