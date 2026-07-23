#include <core/Channel.hpp>

Channel::Channel() {}

// Join command :
Channel::Channel(Client * client, std::string name) : _name(name) {
    _clients.push_back(client);
}

void    Channel::addClient(Client* newClient) {
    this->_clients.push_back(newClient);
}

bool    Channel::removeClient(Client* oldClient) {
    std::vector<Client *>::iterator it = this->_clients.begin();
    std::vector<Client *>::iterator ite = this->_clients.end();

    while(it != ite) {
        if (*it == oldClient) {
            this->_clients.erase(it);
            return true;
        }
        ++it;
    }

    return false;

}

void    Channel::setTopic(std::string newTopic) {
    _topic = newTopic;
    return;
}

std::string Channel::getTopic() {
    return _topic;
}

Channel::~Channel() {}