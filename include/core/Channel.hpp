#pragma once

#include <iostream>
#include <vector>

class Client;

class Channel {
    private:

        std::string             _name;
        std::string             _topic;
        
    public:

        std::vector<Client*>    _clients;
        
        Channel();
        Channel(Client * client, std::string name);

        void    addClient(Client* newClient);
        void    removeClient(Client* oldClient);
        Client* findClient(Client& client);

        void                    setName(std::string& newName);
        const std::string&      getName() const;

        void                    setTopic(std::string& newTopic);
        const std::string&      getTopic() const;

        ~Channel();
};