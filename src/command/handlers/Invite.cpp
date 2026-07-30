#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

void    CommandHandler::_invite(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 2)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    
    Client* invited = _server.getClientByNick(cmd.params[0]);
    if (!invited)
        return client.fillOutBuffer(Reply::noSuchNick(client, cmd.params[0]).c_str(), _server.getEfd());
    
    Channel* targetChannel = _server.getChannelByName(cmd.params[1]);
    if (!targetChannel)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[1]).c_str(), _server.getEfd());
    
    if (!targetChannel->findClient(client))
        return client.fillOutBuffer(Reply::notOnChannel(client, cmd.params[1]).c_str(), _server.getEfd());
    
    if (targetChannel->i) {
        if (!targetChannel->isOperator(client))
            return client.fillOutBuffer(Reply::chanOprivsNeeded(client, cmd.params[1]).c_str(), _server.getEfd());
    }

    if (targetChannel->findClient(*invited))
        return client.fillOutBuffer(Reply::userOnChannel(client, cmd.params[0], cmd.params[1]).c_str(), _server.getEfd());

    targetChannel->addWhitelist(invited->getNick());
    
    client.fillOutBuffer(Reply::inviting(client, invited->getNick(), targetChannel->getName()).c_str(), _server.getEfd());
    return invited->fillOutBuffer(Reply::relayInvite(client, invited->getNick(), targetChannel->getName()).c_str(), _server.getEfd());
}