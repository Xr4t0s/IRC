#pragma once

#include <iostream>
#include <vector>

class Client;

class Channel {
    private:

        std::string             _name;
        
    public:

        std::vector<Client*>    _clients;
        
        Channel();
        Channel(Client * client, std::string name);
        ~Channel();
};