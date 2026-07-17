#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd), _hasPassword(false), _hasUsername(false), _hasNick(false), registered(false) {}

std::string Client::extractCommand() {
    size_t index = _inBuff.find("\r\n");

    std::string cmd = _inBuff.substr(0, index);
    _inBuff.erase(0, index + 2);

    return cmd;
}

std::string& Client::getOutBuff()
{
    return _outBuff;
}

std::string& Client::getInBuff()
{
    return _inBuff;
}

const std::string& Client::getNick() const
{
    return _nick;
}

void Client::setNick(std::string newNick)
{
    _hasNick = true;
    _nick = newNick;
}

const std::string& Client::getUser() const
{
    return _user;
}

void Client::setUser(std::string newUser)
{
    _hasUsername = true;
    _user = newUser;
}

void    Client::fillInBuffer(const char* buff) {
        _inBuff.append(buff);
}

void    Client::fillOutBuffer(const char* buff, int efd) {
    bool wasEmpty = _outBuff.empty();
    _outBuff.append(buff);

    if (wasEmpty)
    {
        epoll_event event;
        event.events = EPOLLIN | EPOLLOUT;
        event.data.fd = this->_fd;
        epoll_ctl(efd, EPOLL_CTL_MOD, this->_fd, &event);
    }
}

bool    Client::hasCompleteCommand() const {
    size_t index = _inBuff.find("\r\n");

    if (index != std::string::npos)
        return true;

    return false;
}

Client::~Client() {}