#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

void CommandHandler::_quit(Client& client, const Command& cmd) {
    std::map<std::string, Client*> list;
    std::string trail;

    if (cmd.params.size() < 1)
        trail = "Client quit";
    for (size_t i = 0; i < client.channels.size(); i++)
    {
        for (size_t y = 0; y < client.channels[i]->clients.size(); y++)
        {
            Client * tmp = client.channels[i]->clients[y];
            if (client.getNick() != tmp->getNick())
            {
                if(list.find(tmp->getNick()) == list.end())
                    list.insert(std::make_pair(tmp->getNick(), tmp));
            }
        }
        
        if (client.channels[i]->clients.size() - 1 == 0)
            _server.deleteChannel(client.channels[i]);

    }

    std::map<std::string, Client*>::iterator it = list.begin();
    std::map<std::string, Client*>::iterator ite = list.end();

    while (it != ite)
    {
        it->second->fillOutBuffer(Reply::relayQuit(client, (trail.empty() ? cmd.params[0] : trail)).c_str(), _server.getEfd());
        it++;
    }

    _server.removeClient(client);
}