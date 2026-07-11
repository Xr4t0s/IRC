#include "command/CommandHandler.hpp"
#include "core/Server.hpp"

//PASS/USER ERR
static const char *ERR_NEEDMOREPARAMS = "461 ERR_NEEDMOREPARAMS";
static const char *ERR_PASSWDMISMATCH = "464 ERR_PASSWDMISMATCH";
static const char *ERR_ALREADYREGISTRED = "462 ERR_ALREADYREGISTRED";

//NICK ERR
static const char *ERR_NONICKNAMEGIVEN  = "431 ERR_NONICKNAMEGIVEN";
// static const char *ERR_ERRONEUSNICKNAME = "432 ERR_ERRONEUSNICKNAME";// not use for now
static const char *ERR_NICKNAMEINUSE    = "433 ERR_NICKNAMEINUSE";
// static const char *ERR_NICKCOLLISION    = "436 ERR_NICKCOLLISION";// not use for now


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

    if (cmd.params.size() < 1)
    {
        client.fillBuffer(ERR_NEEDMOREPARAMS, OUTPUT);
        return;
    }

    if (!cmd.params[0].compare(_server.getPassword()))
        client._hasPassword = true;
    else
        client.fillBuffer(ERR_PASSWDMISMATCH, OUTPUT);
    
    if (client._hasNick && client._hasUsername && client._hasPassword)
        client.registered = true;
}
void CommandHandler::_nick(Client& client, const Command& cmd) {

    if (cmd.params.size() < 1 || cmd.params[0].empty() == true)
    {
        client.fillBuffer(ERR_NONICKNAMEGIVEN, OUTPUT);
        return;
    }
    if (_server.nickExists(cmd.params[0]))
    {
        client.fillBuffer(ERR_NICKNAMEINUSE, OUTPUT); // Or ERR_NICKCOLLISION don't know which one to use.
        return;
    }

    // TODO: if ( nick has invalid/not compatible characters)
    // fillBuffer( ERR_ERRONEUSNICKNAME )
    
    client.setNick(cmd.params[0]);
    std::cout << client.getNick() << std::endl;// console log

    if (client._hasNick && client._hasUsername && client._hasPassword)
        client.registered = true;
}
void CommandHandler::_user(Client& client, const Command& cmd) {
    if (client.registered == true)
    {
        client.fillBuffer(ERR_ALREADYREGISTRED, OUTPUT);
        return;
    }
    if (cmd.params.size() < 1)
    {
        client.fillBuffer(ERR_NEEDMOREPARAMS, OUTPUT);
        return;
    }

    client.setUser(cmd.params[0]);
    std::cout << client.getUser() << std::endl;// console log

    //TODO: handle realName [USER < username > '' '' < :realName > ]

    if (client._hasNick && client._hasUsername && client._hasPassword)
        client.registered = true;
}
void CommandHandler::_join(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
}
void CommandHandler::_quit(Client& client, const Command& cmd) {
    static_cast<void>(client);
    static_cast<void>(cmd);
}
