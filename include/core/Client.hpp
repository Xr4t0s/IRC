#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>

#include "Channel.hpp"

class Client {
    private:
        int                     _fd;
        std::string             _nick;
        std::string             _user;
        std::string             _inBuff;
        std::string             _outBuff;

    public:
        bool                    hasPassword;
        bool                    hasUsername;
        bool                    hasNick;
        bool                    registered;
		std::vector<Channel*>   channels;

        Client(int fd);

        int                     getFd();
        std::string&            getOutBuff();
        std::string&            getInBuff();
        const std::string&      getNick() const;
        const std::string&      getUser() const;

        void                    setNick(std::string newNick);
        void                    setUser(std::string newUser );

        void                    fillInBuffer(const char* buff);
        void                    fillOutBuffer(const char* buff, int efd);
        bool                    hasCompleteCommand() const;
        std::string             extractCommand();
        
        void                    removeChannel(Channel* oldChannel);

        ~Client();
};