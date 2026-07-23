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
        bool    removeClient(Client* oldClient);

        void            setTopic(std::string newTopic);
        std::string     getTopic();

        ~Channel();
};