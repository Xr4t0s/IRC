#include <core/Client.hpp>

Client::Client(int fd) : _fd(fd) {}
Client::~Client() {
    close(_fd);
}