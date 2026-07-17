#pragma once

#include <iostream>

class Reply {
    public:
        Reply();

        static std::string sendReply(int code, const std::string& target, const std::string& trailing);

        ~Reply();
};