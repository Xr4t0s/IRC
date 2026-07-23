#include "command/Reply.hpp"
#include "utils/Utils.hpp"
#include "core/Client.hpp"

Reply::Reply() {}

std::string Reply::_serializeNumeric(int code, const std::string& target, const std::string& trailing) {
    std::string ret = ":irc.server.net " + intToString(code) + " " + (target.empty() ? "*" : target) + " :" + trailing + "\r\n";

    return ret;
}

std::string Reply::_serializeBroadcast(int code, const std::string& target, const std::vector<std::string> params, const std::string& trailing) {
    std::string ret = ":irc.server.net " + intToString(code) + " " + (target.empty() ? "*" : target) + " :" + trailing + "\r\n";
    
    static_cast<void>(params);
    
    return ret;
}

std::string Reply::unknownCommand(Client& client, const std::string& command) {
    return _serializeNumeric(
        421,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_UNKNOWNCOMMAND FOR COMMAND -> " + command
    );
}

std::string Reply::alreadyRegistered(Client& client) {
    return _serializeNumeric(
        462,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_ALREADYREGISTERED"
    );
}

std::string Reply::needMoreParams(Client& client, const std::string& command) {
    return _serializeNumeric(
        461,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NEEDMOREPARAMS FOR COMMAND -> " + command
    );
}

std::string Reply::passwdMismatch(Client& client) {
    return _serializeNumeric(
        464,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_PASSWDMISMATCH"
    );
}

std::string Reply::noSuchChannel(Client& client, const std::string& channel) {
    return _serializeNumeric(
        403,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NOSUCHCHANNEL -> " + channel
    );
}

std::string Reply::welcome(Client& client) {
    return _serializeNumeric(
        1,
        (client.getNick().empty() ? "*" : client.getNick()),
        "Welcome to our IRC server " + client.getNick()
    );
}

std::string Reply::notRegistered(Client& client) {
    return _serializeNumeric(
        451,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NOTREGISTERED"
    );
}

std::string Reply::noNicknameGiven(Client& client) {
    return _serializeNumeric(
        431,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NONICKNAMEGIVEN"
    );
}

std::string Reply::nicknameInUse(Client& client, const std::string& nick) {
    return _serializeNumeric(
        433,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NICKNAMEINUSE -> " + nick
    );
}

std::string Reply::notOnChannel(Client& client, const std::string& channel) {
    return _serializeNumeric(
        442,
        (client.getNick().empty() ? "*" : client.getNick()),
        "ERR_NOTONCHANNEL -> " + channel
    );
}

std::string Reply::namReply(Client& client, const std::string& channel, const std::string& names) {
    return _serializeNumeric(
        353,
        (client.getNick().empty() ? "*" : client.getNick()) + " = " + channel,
        names
    );
}

std::string Reply::endOfNames(Client& client, const std::string& channel) {
    return _serializeNumeric(
        366,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "End of /NAMES list"
    );
}

std::string Reply::relayJoin(Client& src, const std::string& channel) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost JOIN " + channel + "\r\n";
}

std::string Reply::relayNick(Client& src, const std::string& oldNick, const std::string& newNick) {
    return ":" + oldNick + "!" + src.getUser() + "@localhost NICK :" + newNick + "\r\n";
}

std::string Reply::relayPart(Client& src, const std::string& channel, const std::string& reason) {
    std::string ret = ":" + src.getNick() + "!" + src.getUser() + "@localhost PART " + channel;
    if (!reason.empty())
        ret += " :" + reason;
    ret += "\r\n";
    return ret;
}

Reply::~Reply() {}