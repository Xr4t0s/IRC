#include <core/Channel.hpp>

Channel::Channel() {}

// Join command :
Channel::Channel(Client * client, std::string name) : _name(name) {
    _clients.push_back(client);
}

Channel::~Channel() {}