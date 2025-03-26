#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../WebServ.hpp"

class CgiHandler {

    private:
        std::string _key;
        std::string _value;
    
    public:
    
        CgiHandler();
        ~CgiHandler();
        
        void setKey(std::string key);
        void setValue(std::string value);
        
        std::string getKey() const;
        std::string getValue() const;
        
        void parseCgi(std::string cgiLine);
};

#endif
