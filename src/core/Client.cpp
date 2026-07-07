#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd), _hasPassword(false), _hasUsername(false), registered(false) {}

void    Client::fillBuffer(char* buff) {
    _buff.append(buff);
}

bool    Client::hasCompleteCommand() const {
    size_t index = _buff.find("\r\n");

    if (index != std::string::npos)
        return true;

    return false;
}

std::string Client::extractCommand() {
    size_t index = _buff.find("\r\n");

    std::string cmd = _buff.substr(0, index);
    _buff.erase(0, index + 2);

    return cmd;
}

Client::~Client() {}