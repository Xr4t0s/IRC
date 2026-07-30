#pragma once

#include <iostream>
#include <vector>

struct Command {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

Command parseCommand(const std::string& str);