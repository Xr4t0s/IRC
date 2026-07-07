#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "core/Server.hpp"
#include "core/Client.hpp"

struct Command {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

class CommandHandler {
    private:
        Server& _server;
        typedef void (CommandHandler::*Handler)(Client& client, const Command& cmd);
        std::map<std::string, Handler>  _cmds;

        void    _pass(Client& client, const Command& cmd);
        void    _nick(Client& client, const Command& cmd);
        void    _join(Client& client, const Command& cmd);
        void    _user(Client& client, const Command& cmd);
        void    _quit(Client& client, const Command& cmd);

    public:
        CommandHandler(Server& server);

        void execute(Client& client, const Command& cmd);

        ~CommandHandler();
};

Command parseCommand(const std::string& str);