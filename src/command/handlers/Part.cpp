#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void CommandHandler::_part(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    
    std::vector<std::string> channelsToQuit = splitBy(cmd.params[0], ',');
    for (size_t i = 0; i < channelsToQuit.size(); i++) {
        Channel* channelToQuit = _server.getChannelByName(channelsToQuit[i]);
        if (!channelToQuit) {
            client.fillOutBuffer(Reply::noSuchChannel(client, channelsToQuit[i]).c_str(), _server.getEfd());
            continue;
        }
            
        if (channelToQuit->findClient(client)) {
            std::string reason = (cmd.params.size() == 2 ? cmd.params[1] : "");
            std::string channelName = channelToQuit->getName();
            for (size_t x = 0; x < channelToQuit->clients.size(); x++)
                channelToQuit->clients[x]->fillOutBuffer(Reply::relayPart(client, channelName, reason).c_str(), _server.getEfd());
            if (channelToQuit->isOperator(client))
                channelToQuit->removeOperator(&client);

            if (channelToQuit->isWhitelisted(client.getNick()))
                channelToQuit->removeWhitelist(client.getNick());

            channelToQuit->removeClient(&client);
            client.removeChannel(channelToQuit);
            if (channelToQuit->clients.empty())
                _server.deleteChannel(channelToQuit);
        } else
            client.fillOutBuffer(Reply::notOnChannel(client, channelToQuit->getName()).c_str(), _server.getEfd());
    }

}