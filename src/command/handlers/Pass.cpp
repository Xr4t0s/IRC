#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

void CommandHandler::_pass(Client& client, const Command& cmd) {
    if (client.registered == true)
        return client.fillOutBuffer(Reply::alreadyRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    if (!cmd.params[0].compare(_server.getPassword()))
        client.hasPassword = true;
    else {
        client.hasPassword = false;
        client.fillOutBuffer(Reply::passwdMismatch(client).c_str(), _server.getEfd());
    }
    
    return completeRegistration(client);
}