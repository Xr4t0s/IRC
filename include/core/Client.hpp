#pragma once

#include <iostream>
#include <unistd.h>

class Client {
    private:
        int         _fd;
        bool        _hasPassword;
        bool        _hasUsername;
        
        std::string _nick;
        std::string _user;
        std::string _inBuff;
        std::string _outBuff;

    public:
        bool        registered;
        
        Client(int fd);

        void        fillBuffer(const char* buff, bool io);
        bool        hasCompleteCommand() const;
        std::string extractCommand();
        
        ~Client();
};