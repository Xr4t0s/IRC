#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void CommandHandler::_topic(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    Channel * channel = _server.getChannelByName(cmd.params[0]);
    if (channel == NULL)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[0]).c_str(), _server.getEfd());
    if (channel->findClient(client) == NULL)
        return client.fillOutBuffer(Reply::notOnChannel(client, channel->getName()).c_str(), _server.getEfd());
    if (cmd.params.size() < 2) {
        if (channel->getTopic().empty())
            return client.fillOutBuffer(Reply::noTopic(client, channel->getName()).c_str(), _server.getEfd());
        else
            return client.fillOutBuffer(Reply::topic(client, channel->getName(), channel->getTopic()).c_str(), _server.getEfd());
    }
    if (channel->t) {
        if (!(channel->isOperator(client)))
            return client.fillOutBuffer(Reply::chanOprivsNeeded(client, channel->getName()).c_str(), _server.getEfd());
    }
    channel->setTopic(cmd.params[1]);
    for (size_t i = 0; i < channel->clients.size(); i++)
        channel->clients[i]->fillOutBuffer(Reply::relayTopic(client, channel->getName(), cmd.params[1]).c_str(), _server.getEfd());
}