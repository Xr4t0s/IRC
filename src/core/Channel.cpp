#include "core/Channel.hpp"
#include "core/Client.hpp"

Channel::Channel() : _name("Random name"), _topic("Random topic") {}

// Join command :
Channel::Channel(Client * client, std::string name) : _name(name), _topic("Random topic") {
    _clients.push_back(client);
}

void    Channel::addClient(Client* newClient) {
    this->_clients.push_back(newClient);
}

void    Channel::removeClient(Client* oldClient) {
    std::vector<Client *>::iterator it = this->_clients.begin();
    std::vector<Client *>::iterator ite = this->_clients.end();

    while(it != ite) {
        if (*it == oldClient) {
            this->_clients.erase(it);
        }
        ++it;
    }
}

Client* Channel::findClient(Client& client) {
    std::vector<Client *>::iterator it = _clients.begin();
    std::vector<Client *>::iterator ite = _clients.end();

    while (it != ite) {
        if ((*it)->getNick() == client.getNick()) {
            return *it;
        }
        it++;
    }
    return NULL;
}

void    Channel::setName(std::string& newName) {
    _name = newName;
}

const std::string& Channel::getName() const {
    return _name;
}

void    Channel::setTopic(std::string& newTopic) {
    _topic = newTopic;
    return;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

Channel::~Channel() {}