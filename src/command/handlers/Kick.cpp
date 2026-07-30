#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void CommandHandler::_kick(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());


    if(cmd.params.size() < 2)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    std::string comment = (cmd.params.size() == 3 ? cmd.params[2] : client.getNick());

    Channel* channel = _server.getChannelByName(cmd.params[0]);
    std::vector<std::string> users = splitBy(cmd.params[1], ',');

	if (cmd.params[0].empty() || users.empty())
		return ;

    if(!channel)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[0]).c_str(), _server.getEfd());
    
    if (!channel->isOperator(client))
            return client.fillOutBuffer(Reply::chanOprivsNeeded(client, channel->getName()).c_str(), _server.getEfd());

    if (!channel->findClient(client))
        return client.fillOutBuffer(Reply::notOnChannel(client, channel->getName()).c_str(), _server.getEfd());

    for (size_t x = 0; x < users.size(); x++) {
        Client * target = _server.getClientByNick(users[x]);
        if (!target || !channel->findClient(*target)) {
            client.fillOutBuffer(Reply::userNotInChannel(client, users[x], channel->getName()).c_str(), _server.getEfd());
            continue;
        }
        for (size_t y = 0; y < channel->clients.size(); y++)
            channel->clients[y]->fillOutBuffer(Reply::relayKick(client, channel->getName(), users[x], comment).c_str(), _server.getEfd());
        channel->removeClient(target);
        target->removeChannel(channel);
        if (channel->isOperator(*target))
            channel->removeOperator(target);
    }
}