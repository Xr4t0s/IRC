#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void CommandHandler::_privmsg(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::noRecipient(client, cmd.command).c_str(), _server.getEfd());
    if (cmd.params.size() < 2 || cmd.params[1].empty())
        return client.fillOutBuffer(Reply::noTextToSend(client).c_str(), _server.getEfd());
    
    std::vector<std::string> tmp = splitBy(cmd.params[0], ',');
    std::vector<std::string>::iterator it = tmp.begin();
    while (it != tmp.end())
    {
        if ((*it)[0] == '&' || (*it)[0] == '#')
        {
            Channel * channel = _server.getChannelByName((*it));

            if (channel == NULL) {
                client.fillOutBuffer(Reply::noSuchChannel(client, *it).c_str(), _server.getEfd());
                it++;
                continue;
            }
            
            if (channel->findClient(client) == NULL) {
                client.fillOutBuffer(Reply::cannotSendToChan(client, *it).c_str(), _server.getEfd());
                it++;
                continue;
            }

            for (size_t i = 0; i < channel->clients.size(); i++)
            {
                if (client.getNick() != channel->clients[i]->getNick())
                    channel->clients[i]->fillOutBuffer(Reply::relayPrivmsg(client, *it, cmd.params[1]).c_str(), _server.getEfd());
            }
        }
        else
        {
            Client * target = _server.getClientByNick(*it);
            if (target == NULL) {
                client.fillOutBuffer(Reply::noSuchNick(client, *it).c_str(), _server.getEfd());
                it++;
                continue;
            }
            target->fillOutBuffer(Reply::relayPrivmsg(client, (*it), cmd.params[1]).c_str(), _server.getEfd()); 
        }
        it++;
    }
}