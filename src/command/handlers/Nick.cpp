#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

void CommandHandler::_nick(Client& client, const Command& cmd) {
    std::map<std::string, Client*> list;

    if (cmd.params.size() < 1 || cmd.params[0].empty() == true)
        return client.fillOutBuffer(Reply::noNicknameGiven(client).c_str(), _server.getEfd());

    const std::string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-[]\\`_^{}|";

    if (std::isdigit(static_cast<unsigned char>(cmd.params[0][0])) || cmd.params[0][0] == '-')
        return client.fillOutBuffer(Reply::erroneusNickname(client, cmd.params[0]).c_str(), _server.getEfd());
    
    for (size_t i = 0; i < cmd.params[0].size(); i++)
    {
        if (allowed.find(cmd.params[0][i]) == std::string::npos)
            return client.fillOutBuffer(Reply::erroneusNickname(client, cmd.params[0]).c_str(), _server.getEfd());
    }

    if (_server.getClientByNick(cmd.params[0]) != NULL && client.getNick() != cmd.params[0])
        return client.fillOutBuffer(Reply::nicknameInUse(client, cmd.params[0]).c_str(), _server.getEfd());

    std::string oldNick = client.getNick();
    client.setNick(cmd.params[0]);
	if (client.channels.empty())
		client.fillOutBuffer(Reply::relayNick(client, oldNick, client.getNick()).c_str(), _server.getEfd());

    for (size_t i = 0; i < client.channels.size(); i++)
    {
        for (size_t y = 0; y < client.channels[i]->clients.size(); y++)
        {
            Client * tmp = client.channels[i]->clients[y];
            if(list.find(tmp->getNick()) == list.end())
                list.insert(std::make_pair(tmp->getNick(), tmp));
        }
    }

    if (client.registered) {    
        std::map<std::string, Client*>::iterator it = list.begin();
        std::map<std::string, Client*>::iterator ite = list.end();
        
        while (it != ite)
        {
            it->second->fillOutBuffer(Reply::relayNick(client, oldNick, client.getNick()).c_str(), _server.getEfd());
            it++;
        }
    }

    return completeRegistration(client);
}