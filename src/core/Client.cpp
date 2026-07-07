#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd), _hasPassword(false), _hasUsername(false), registered(false) {}

void    Client::fillBuffer(char* buff)
{
    _buff.append(buff);

    size_t index = _buff.find("\r\n");

    if (index != std::string::npos)
    {
        // _hasCompleteCommand = true;
        _buff = _buff.substr(index + 2, _buff.length());
    }
}

Client::~Client() {
}