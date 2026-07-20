#pragma once

#include <iostream>
#include "Client.hpp"

class Channel {
    private:

        std::string             _name;
        std::vector<Client*>    _clients;
    
    public:
        Channel();
        Channel(Client * client, std::string name);
        ~Channel();
};