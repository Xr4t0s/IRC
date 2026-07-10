#include "command/CommandHandler.hpp"
#include "core/Server.hpp"

static const char *ERR_NEEDMOREPARAMS = "461 ERR_NEEDMOREPARAMS";
static const char *ERR_PASSWDMISMATCH = "464 ERR_PASSWDMISMATCH";
static const char *ERR_ALREADYREGISTRED = "462 ERR_ALREADYREGISTRED";


CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["QUIT"] = &CommandHandler::_quit;
}

void CommandHandler::execute(Client& client, const Command& cmd) {
    std::map<std::string, CommandHandler::Handler>::iterator it = _cmds.find(cmd.command);

    if (it == _cmds.end()) return; // TODO! if (!h) h = ERR_UNKNOWNCOMMAND 421

    Handler h = (*it).second;
    (this->*h)(client, cmd);
}

CommandHandler::~CommandHandler() {}

void CommandHandler::_pass(Client& client, const Command& cmd) {
    if (client.registered == true)
    {
        client.fillBuffer(ERR_ALREADYREGISTRED, OUTPUT);
        return;
    }
    if (cmd.params.size() != 1)
    {
        client.fillBuffer(ERR_NEEDMOREPARAMS, OUTPUT);
        return;
    }
    if (!cmd.params[0].compare(_server.getPassword()))
        client.registered = true;
    else
        client.fillBuffer(ERR_PASSWDMISMATCH, OUTPUT);
}
void CommandHandler::_nick(Client& client, const Command& cmd) {






}
void CommandHandler::_user(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
}
void CommandHandler::_join(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
}
void CommandHandler::_quit(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
}
