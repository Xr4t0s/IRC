#pragma once

#include <iostream>
#include <vector>

class Client;

class Channel {
    private:

        std::string                 _name;
        std::string                 _topic;
        std::vector<Client*>        _operator;
        std::vector<std::string>    _whitelist;
        
    public:
    
        int     l;
        bool    i;
        bool    t;
        std::string    k;


        std::vector<Client*>    _clients;
        
        Channel();
        Channel(Client * client, std::string name);

        void    addClient(Client* newClient);
        void    removeClient(Client* oldClient);
        Client* findClient(Client& client);

        void    addOperator(Client* newOperator);
        void    removeOperator(Client* oldOperator);
        bool    isOperator(Client& client);

        void    addWhitelist(std::string nick);
        void    removeWhitelist(std::string nick);
        bool    isWhitelisted(std::string nick);

        void                    setName(std::string& newName);
        const std::string&      getName() const;

        void                    setTopic(std::string& newTopic);
        const std::string&      getTopic() const;

        ~Channel();
};