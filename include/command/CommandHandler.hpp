#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "core/Client.hpp"
#include "command/Command.hpp"

#define OUTPUT 0
#define INPUT 1

class Server;

class CommandHandler {
    private:
        Server& _server;
        typedef void (CommandHandler::*Handler)(Client& client, const Command& cmd);
        std::map<std::string, Handler>  _cmds;

        void    _pass(Client& client, const Command& cmd);
        void    _nick(Client& client, const Command& cmd);
        void    _user(Client& client, const Command& cmd);
        void    _join(Client& client, const Command& cmd);
        void    _part(Client& client, const Command& cmd);
        void    _privmsg(Client& client, const Command& cmd);

    public:
        CommandHandler(Server& server);

        void execute(Client& client, const Command& cmd);
        void completeRegistration(Client& client);

        ~CommandHandler();
};

Command parseCommand(const std::string& str);