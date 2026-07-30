#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["PART"] = &CommandHandler::_part;
    _cmds["PRIVMSG"] = &CommandHandler::_privmsg;
    _cmds["TOPIC"] = &CommandHandler::_topic;
    _cmds["KICK"] = &CommandHandler::_kick;
    _cmds["QUIT"] = &CommandHandler::_quit;
    _cmds["INVITE"] = &CommandHandler::_invite;
    _cmds["MODE"] = &CommandHandler::_mode;

}

void CommandHandler::execute(Client& client, const Command& cmd) {
    std::map<std::string, CommandHandler::Handler>::iterator it = _cmds.find(cmd.command);

    if (it == _cmds.end())
        return client.fillOutBuffer(Reply::unknownCommand(client, cmd.command).c_str(), _server.getEfd());

    Handler h = (*it).second;
    (this->*h)(client, cmd);
}

void CommandHandler::completeRegistration(Client& client) {
    if(client.registered)
        return ;

    if (client.hasNick && client.hasUsername && client.hasPassword)
    {
        client.registered = true;
        return client.fillOutBuffer(Reply::welcome(client).c_str(), _server.getEfd());
    }
}

CommandHandler::~CommandHandler() {}