#pragma once

#include <iostream>
#include <unistd.h>

class Client {
    private:
        int         _fd;
        bool        _hasPassword;            
        bool        _hasUsername;
        
        std::string _name;
        std::string _buff;

    public:
        bool        registered;        
        
        Client(int fd);

        void    fillBuffer(const std::string& buff);

        ~Client();
};