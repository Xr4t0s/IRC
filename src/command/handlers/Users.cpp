#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

void CommandHandler::_user(Client& client, const Command& cmd) {
    if (client.registered == true)
        return client.fillOutBuffer(Reply::alreadyRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    client.setUser(cmd.params[0]);

    //TODO: handle realName [USER < username > '' '' < :realName > ]

    return completeRegistration(client);
}