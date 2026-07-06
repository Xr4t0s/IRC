#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd), _hasPassword(false), _hasUsername(false), registered(false) {}

void    Client::fillBuffer(char* buff)
{
    _buff.append(buff);

    if (_buff.find("\r\n") != std::string::npos)
        std::cout << "npos" << std::endl;
}

Client::~Client() {
}