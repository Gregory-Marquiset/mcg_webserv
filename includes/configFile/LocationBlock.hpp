#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include "../WebServ.hpp"
#include "Block.hpp"
#include "RecupBlockContent.hpp"

class LocationBlock {

    private:
       std::string _path;
       std::string _root;
       std::string _index;
       std::string _allowMethods;
       std::string _cgiExtension;

    public:
        LocationBlock();
        ~LocationBlock();

        /* setters */

        void setPath(std::string path);
        void setRoot(std::string root);
        void setIndex(std::string index);
        void setAllowMethods(std::string allowMethods);
        void setCgiExtension(std::string cgiExtension);
        
        /* getters */

        std::string getPath() const;
        std::string getRoot() const;
        std::string getIndex() const;
        std::string getAllowMethods() const;
        std::string getCgiExtension() const;
};

#endif