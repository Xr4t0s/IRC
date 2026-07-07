#include "command/CommandHandler.hpp"

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
    static_cast<void>(client);
    static_cast<void>(cmd);
}
void CommandHandler::_nick(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
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
