#include "core/Channel.hpp"
#include "core/Client.hpp"

Channel::Channel() : _name("Random name"), _topic("Random topic"), l(-1), i(false), t(false) {}

// Join command :
Channel::Channel(Client * client, std::string name) : _name(name), _topic("Random topic"), l(-1), i(false), t(false) {
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

void    Channel::addOperator(Client* newOperator) {
    this->_operator.push_back(newOperator);
}

void    Channel::removeOperator(Client* oldOperator) {
    std::vector<Client *>::iterator it = this->_operator.begin();
    std::vector<Client *>::iterator ite = this->_operator.end();

    while(it != ite) {
        if (*it == oldOperator) {
            this->_operator.erase(it);
        }
        ++it;
    }
}

bool    Channel::isOperator(Client& client) {
    std::vector<Client *>::iterator it = _operator.begin();
    std::vector<Client *>::iterator ite = _operator.end();

    while (it != ite) {
        if ((*it)->getNick() == client.getNick()) {
            return true;
        }
        it++;
    }
    return false;
}

void    Channel::addWhitelist(std::string nick) {
    this->_whitelist.push_back(nick);
}

void    Channel::removeWhitelist(std::string nick) {
    std::vector<std::string>::iterator it = this->_whitelist.begin();
    std::vector<std::string>::iterator ite = this->_whitelist.end();

    while(it != ite) {
        if (*it == nick) {
            this->_whitelist.erase(it);
        }
        ++it;
    }
}

bool    Channel::isWhitelisted(std::string nick) {
    std::vector<std::string>::iterator it = this->_whitelist.begin();
    std::vector<std::string>::iterator ite = this->_whitelist.end();

    while(it != ite) {
        if (*it == nick) {
            return true;
        }
        ++it;
    }
    return false;
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