#pragma once

#include <iostream>
#include <map>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <signal.h>

#ifndef MAX_EVENTS
    # define MAX_EVENTS 64
#endif

#include "core/Client.hpp"
#include "core/Channel.hpp"
#include "utils/Error.hpp"
#include "command/CommandHandler.hpp"

class Server {
    public:
        Server();
        
        bool        init(char **args);
        void        run();

        Channel*    getChannelByName(const std::string& name);
        Client*     getClientByFd(int fd);

        ~Server();

    private:
        int                             _fd;
        std::string                     _pass;
        int                             _port;
        int                             _efd;

        std::map<int, Client>           _clients;
        std::map<std::string, Channel>  _channels;

        CommandHandler                  _cmdHandler;

        void    _init_pass(char *pass);
        void    _init_port(char *port);
        void    _init_socket();
        void    _init_listening();
        void    _init_epoll();

        void    _accept_client();
        void    _remove_client(int fd);
};