#pragma once

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#ifndef MAX_EVENTS
    # define MAX_EVENTS 64
#endif

#include "utils/Error.hpp"

class Server {
    public:
        Server();
        bool    init(char **args);
        void    run();
        ~Server();

    private:
        int             _fd;
        std::string     _pass;
        int             _port;

        int             _efd;

        void    _init_pass(char *pass);
        void    _init_port(char *port);
        void    _init_socket();
        void    _init_listening();
        void    _init_epoll();

        void    _accept_client();
        void    _remove_client(int fd);
};