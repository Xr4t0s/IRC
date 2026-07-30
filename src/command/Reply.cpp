#include "command/Reply.hpp"
#include "utils/Utils.hpp"
#include "core/Client.hpp"

Reply::Reply() {}

std::string Reply::_serializeNumeric(int code, const std::string& target, const std::string& trailing) {
    std::string ret = ":irc.server.net " + intToString(code) + " " + (target.empty() ? "*" : target)
        + (trailing.empty() ? "\r\n" : " :" + trailing + "\r\n");

    return ret;
}

std::string Reply::unknownCommand(Client& client, const std::string& command) {
    return _serializeNumeric(
        421,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + command,
        "Unknown command"
    );
}

std::string Reply::alreadyRegistered(Client& client) {
    return _serializeNumeric(
        462,
        (client.getNick().empty() ? "*" : client.getNick()),
        "You may not reregister"
    );
}

std::string Reply::needMoreParams(Client& client, const std::string& command) {
    return _serializeNumeric(
        461,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + command,
        "Not enough parameters"
    );
}

std::string Reply::noNicknameGiven(Client& client) {
    return _serializeNumeric(
        431,
        (client.getNick().empty() ? "*" : client.getNick()),
        "No nickname given"
    );
}

std::string Reply::erroneusNickname(Client& client, const std::string& badNick) {
    return _serializeNumeric(
        432,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + badNick,
        "Erroneus nickname"
    );
}

std::string Reply::passwdMismatch(Client& client) {
    return _serializeNumeric(
        464,
        (client.getNick().empty() ? "*" : client.getNick()),
        "Password incorrect"
    );
}

std::string Reply::noSuchChannel(Client& client, const std::string& channel) {
    return _serializeNumeric(
        403,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "No such channel"
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
        "You have not registered"
    );
}

std::string Reply::nicknameInUse(Client& client, const std::string& nick) {
    return _serializeNumeric(
        433,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + nick,
        "Nickname collision KILL"
    );
}

std::string Reply::notOnChannel(Client& client, const std::string& channel) {
    return _serializeNumeric(
        442,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "You're not on that channel"
    );
}

std::string Reply::inviteOnlyChan(Client& client, const std::string& channel) {
    return _serializeNumeric(
        473,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "You're not on that channel"
    );
}

std::string Reply::userNotInChannel(Client& client, const std::string& nick, const std::string& channel) {
    return _serializeNumeric(
        441,
        (client.getNick().empty() ? "*" : client.getNick())+ " " + nick + " " + channel,
        "They aren't on that channel"
    );
}

std::string Reply::userOnChannel(Client& client, const std::string& nick, const std::string& channel) {
    static_cast<void>(client);
    return _serializeNumeric(
        443,
        (nick.empty() ? "*" : nick) + " " + channel,
        "User already on the channel"
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

std::string Reply::noRecipient(Client& client, const std::string& command) {
    return _serializeNumeric(
        411,
        (client.getNick().empty() ? "*" : client.getNick()),
        ":No recipient given (" + command + ")"
    );
}

std::string Reply::noTextToSend(Client& client) {
    return _serializeNumeric(
        412,
        (client.getNick().empty() ? "*" : client.getNick()),
        "No text to send"
    );
}

std::string Reply::noSuchNick(Client& client, const std::string& nick) {
    return _serializeNumeric(
        401,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + nick,
        "No such nick/channel"
    );
}

std::string Reply::channelIsFull(Client& client, const std::string& channel) {
    return _serializeNumeric(
        471,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "Cannot join channel (+l)"
    );
}

std::string Reply::cannotSendToChan(Client& client, const std::string& channel) {
    return _serializeNumeric(
        404,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "Cannot send to channel"
    );
}

std::string Reply::noTopic(Client& client, const std::string& channel) {
    return _serializeNumeric(
        331,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "No topic is set"
    );
}

std::string Reply::topic(Client& client, const std::string& channel, const std::string& topic) {
    return _serializeNumeric(
        332,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        topic
    );
}

std::string Reply::chanOprivsNeeded(Client& client, const std::string& channel) {
    return _serializeNumeric(
        482,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "You're not channel operator"
    );
}

std::string Reply::inviting(Client& client, const std::string& targetNick, const std::string& channel) {
    return _serializeNumeric(
        341,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + targetNick + " " + channel,
        "Invited " + targetNick
    );
}

std::string Reply::channelModeIs(Client& client, const std::string& channel, const std::string& modes) {
    return _serializeNumeric(
        324,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel + " " + modes,
        ""
    );
}

std::string Reply::unknownMode(Client& client, char mode) {
    return _serializeNumeric(
        472,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + std::string(1, mode),
        "is unknown mode char to me"
    );
}

std::string Reply::badChannelKey(Client& client, const std::string& channel) {
    return _serializeNumeric(
        475,
        (client.getNick().empty() ? "*" : client.getNick()) + " " + channel,
        "Cannot join channel (+k)"
    );
}

std::string Reply::relayMode(Client& src, const std::string& channel, const std::string& changes) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost MODE " + channel + " " + changes + "\r\n";
}

std::string Reply::relayJoin(Client& src, const std::string& channel) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost JOIN " + channel + "\r\n";
}

std::string Reply::relayTopic(Client& src, const std::string& channel, const std::string& topic) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost TOPIC " + channel + " :" + topic + "\r\n";
}

std::string Reply::relayPrivmsg(Client& src, const std::string& target, const std::string& text) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost PRIVMSG " + target + " :" + text + "\r\n";
}

std::string Reply::relayNick(Client& src, const std::string& oldNick, const std::string& newNick) {
    return ":" + oldNick + "!" + src.getUser() + "@localhost NICK :" + newNick + "\r\n";
}

std::string Reply::relayKick(Client& src, const std::string& channel, const std::string& targetNick, const std::string& comment) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost KICK " + channel + " " + targetNick + " :" + comment + "\r\n";
}

std::string Reply::relayQuit(Client& src, const std::string& reason) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost QUIT :" + reason + "\r\n";
}

std::string Reply::relayPart(Client& src, const std::string& channel, const std::string& reason) {
    std::string ret = ":" + src.getNick() + "!" + src.getUser() + "@localhost PART " + channel;
    if (!reason.empty())
        ret += " :" + reason;
    ret += "\r\n";
    return ret;
}

std::string Reply::relayInvite(Client& src, const std::string& targetNick, const std::string& channel) {
    return ":" + src.getNick() + "!" + src.getUser() + "@localhost INVITE " + targetNick + " " + channel + "\r\n";
}

Reply::~Reply() {}