#pragma once

#include <iostream>
#include <vector>

struct Command {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

// L'idée ici c'est que Command::command == soit NICK soit PASS soit... etc, et on fera tout simplement _cmds[cmd.command]
Command parseCommand(const std::string& str);