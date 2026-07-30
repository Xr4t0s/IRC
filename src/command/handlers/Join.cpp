#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void CommandHandler::_join(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    std::vector<std::string> channels = splitBy(cmd.params[0], ',');
    std::vector<std::string> passwords;
    if (cmd.params.size() > 1)
        passwords = splitBy(cmd.params[1], ',');
    std::size_t consumed = 0;
    for (size_t i = 0; i < channels.size(); i++) {
        std::string name = channels[i];

        if (name.empty() || (name[0] != '#' && name[0] != '&')) {
            client.fillOutBuffer(Reply::noSuchChannel(client, name).c_str(), _server.getEfd());
            continue;
        }

        Channel* channel = _server.getChannelByName(name);
        if (!channel) {
            Channel newChannel(&client, name);
            _server.createNewChannel(&client, name, newChannel);
            channel = _server.getChannelByName(name);
            channel->addOperator(&client);
        } else {
            if (channel->findClient(client))
                return ;
            if (!channel->k.empty()) {
                if (passwords.size() <= consumed || passwords[consumed++] != channel->k) {
                    client.fillOutBuffer(Reply::badChannelKey(client, channel->getName()).c_str(), _server.getEfd());
                    continue;
                }
            }
            if (channel->i) {
                if (!channel->isWhitelisted(client.getNick()))
                    return client.fillOutBuffer(Reply::inviteOnlyChan(client, channel->getName()).c_str(), _server.getEfd());
                channel->removeWhitelist(client.getNick());
            }
            if (channel->l != -1 && channel->l < static_cast<int>(channel->clients.size()) + 1)
                return client.fillOutBuffer(Reply::channelIsFull(client, channel->getName()).c_str(), _server.getEfd());
            channel->addClient(&client);
            client.channels.push_back(channel);
        }

        for (size_t i = 0; i < channel->clients.size(); i++)
            channel->clients[i]->fillOutBuffer(Reply::relayJoin(client, name).c_str(), _server.getEfd());

        if (channel->getTopic().empty())
            client.fillOutBuffer(Reply::noTopic(client, channel->getName()).c_str(), _server.getEfd());
        else
            client.fillOutBuffer(Reply::topic(client, channel->getName(), channel->getTopic()).c_str(), _server.getEfd());
        
        std::string names;
        for (size_t i = 0; i < channel->clients.size(); i++)
            names += (channel->isOperator(*channel->clients[i]) ? "@" : "") + channel->clients[i]->getNick() + " ";

        client.fillOutBuffer(Reply::namReply(client, name, names).c_str(), _server.getEfd());
        client.fillOutBuffer(Reply::endOfNames(client, name).c_str(), _server.getEfd());
    }
}