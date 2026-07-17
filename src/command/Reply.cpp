#include "command/Reply.hpp"
#include "utils/Utils.hpp"

Reply::Reply() {}

std::string Reply::sendReply(int code, const std::string& target, const std::string& trailing) {
    std::string ret = ":irc.server.net " + intToString(code) + " " + (target.empty() ? "*" : target) + " :" + trailing + "\r\n";

    return ret;
}

Reply::~Reply() {}