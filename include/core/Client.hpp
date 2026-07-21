#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include "Channel.hpp"

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

		std::vector<Channel*> channels;

        std::string extractCommand();

        std::string& getOutBuff();
        std::string& getInBuff();

        const std::string& getNick() const;
        void setNick(std::string newNick);

        const std::string& getUser() const;
        void setUser(std::string newUser);

        void        fillInBuffer(const char* buff);
        void        fillOutBuffer(const char* buff, int efd);
        bool        hasCompleteCommand() const;
        
        ~Client();
};