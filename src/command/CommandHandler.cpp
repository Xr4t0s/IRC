#include "command/CommandHandler.hpp"

CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["QUIT"] = &CommandHandler::_quit;
}

CommandHandler::~CommandHandler() {}

void CommandHandler::_pass(Client& client, const Command& cmd) {}
void CommandHandler::_nick(Client& client, const Command& cmd) {}
void CommandHandler::_user(Client& client, const Command& cmd) {}
void CommandHandler::_join(Client& client, const Command& cmd) {}
void CommandHandler::_quit(Client& client, const Command& cmd) {}