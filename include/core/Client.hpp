#pragma once

#include <iostream>
#include <unistd.h>

class Client {
    private:
        int         _fd;
        
        std::string _nick;
        std::string _user;
        std::string _inBuff;
        std::string _outBuff;
        
    public:
        bool        _hasPassword;
        bool        _hasUsername;
        bool        _hasNick;

        bool        registered;
        
        Client(int fd);

        const std::string& getNick() const;
        void setNick(std::string newNick);

        const std::string& getUser() const;
        void setUser(std::string newUser);

        void        fillBuffer(const char* buff, bool io);
        bool        hasCompleteCommand() const;
        std::string extractCommand();
        
        ~Client();
};