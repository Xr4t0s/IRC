#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd), _hasPassword(false), _hasUsername(false), _hasNick(false), registered(false) {}

void    Client::fillBuffer(const char* buff, bool io) {
    if (io == false)// Output
    {
        bool wasEmpty = _outBuff.empty();
        _outBuff.append(buff);

        if (wasEmpty)
        {
            //enable client write
        }
    }
    else
        _inBuff.append(buff);
}

bool    Client::hasCompleteCommand() const {
    size_t index = _inBuff.find("\r\n");

    if (index != std::string::npos)
        return true;

    return false;
}

const std::string& Client::getNick() const
{
    return _nick;
}

void Client::setNick(std::string newNick)
{
    _nick = newNick;
}

const std::string& Client::getUser() const
{
    return _user;
}

void Client::setUser(std::string newUser)
{
    _user = newUser;
}

std::string Client::extractCommand() {
    size_t index = _inBuff.find("\r\n");

    std::string cmd = _inBuff.substr(0, index);
    _inBuff.erase(0, index + 2);

    return cmd;
}

Client::~Client() {}