#include "command/CommandHandler.hpp"

CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["QUIT"] = &CommandHandler::_quit;
}

void CommandHandler::execute(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
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


// L'idée ici c'est que Command::command == soit NICK soit PASS soit... etc, et on fera tout simplement _cmds[cmd.command]
Command parseCommand(const char* str) {
    static_cast<void>(str);
    
    // TODO! create command parser here
}