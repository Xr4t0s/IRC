#include "core/Server.hpp"

Server::Server() : _fd(-1), _port(-1), _efd(-1) {}

bool Server::init(char **args) {
    std::cout << "Server stack created\n";
    
    try {
        _init_pass(args[0]);
        _init_port(args[1]);
        _init_socket();
        _init_listening();
        _init_epoll();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

void Server::run() {
    epoll_event events[MAX_EVENTS];

    while(true) {
        int counts = epoll_wait(_efd, events, MAX_EVENTS, -1);
        if (counts == -1)
            throw EpollError();

        for (int i = 0; i < counts; i++) {
            int fd = events[i].data.fd;

            if (fd == _fd) {
                std::cout << "New connection\n";
                sockaddr addr;
                socklen_t sock_len = sizeof(addr); 
                int client_fd = accept(_fd, &addr, &sock_len);
                if (client_fd == -1) {
                    std::cout << "Failed to accept client\n";
                    continue;
                }

                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
                    std::cout << "Failed to set client as non-blocking\n";
                    close(client_fd);
                    continue;
                }

                epoll_event cli_event;
                cli_event.events = EPOLLIN;
                cli_event.data.fd = client_fd;

                if (epoll_ctl(_efd, EPOLL_CTL_ADD, client_fd, &cli_event) == -1) {
                    std::cout << "Failed to add client to watchlist\n";
                    close(client_fd);
                }

            } else {
                char buff[512 + 1];

                int nread = recv(fd, buff, 512, 0);
                if (nread <= 0) {
                    std::cout << "Removed client" << std::endl;
                    epoll_ctl(_efd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }
                buff[nread] = '\0';
                std::cout << buff << std::endl;
            }
        }
    }
}

void Server::_init_pass(char *pass) {
    _pass = std::string(pass);
    if (_pass.length() < 8)
        throw IllegalPassword();
}

void Server::_init_port(char *port) {
    for (int i = 0; port[i]; i++) {
        if (!isdigit(port[i]))
            throw IllegalPort();
    }

    _port = atoi(port);
    if (_port < 1024 || _port > 65535)
        throw IllegalPort();
}

void Server::_init_socket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1)
        throw SocketError();
    
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(_fd);
        throw SocketError();
    }

    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
        close(_fd);
        throw SocketError();
    }
}

void Server::_init_listening() {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    for (int i = 0; i < 8; i++) {
        addr.sin_zero[i] = 0;
    }

    socklen_t sock_len = sizeof(addr);
    if (bind(_fd, reinterpret_cast<sockaddr*>(&addr), sock_len) == -1) {
        close(_fd);
        throw ListeningError();
    }

    if (listen(_fd, SOMAXCONN) == -1) {
        close(_fd);
        throw ListeningError();
    }
}

void Server::_init_epoll() {
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = _fd;

    _efd = epoll_create1(0);

    if (_efd == -1) {
        close(_fd);
        throw EpollError();
    }

    if (epoll_ctl(_efd, EPOLL_CTL_ADD, _fd, &event) == -1) {
        close(_fd);
        throw EpollError();
    }
}

Server::~Server() {
    if (_fd != -1) {
        close(_fd);
    };
    if (_efd != -1) {
        close(_efd);
    }
    std::cout << "Server stack destroyed\n";
}

const char* Server::IllegalPassword::what() const throw() {
    return "Password should not be less than 8 characters";
}
const char* Server::IllegalPort::what() const throw() {
    return "Port should be between 1024 and 65535";
}
const char* Server::SocketError::what() const throw() {
    return "Failed to create or configure socket";
}
const char* Server::ListeningError::what() const throw() {
    return "Failed to bind or listen server address";
}
const char* Server::EpollError::what() const throw() {
    return "Failed to create or configure epoll";
}