#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"

//PASS/USER ERR
// static const char *ERR_NEEDMOREPARAMS = "461 ERR_NEEDMOREPARAMS";
// static const char *ERR_PASSWDMISMATCH = "464 ERR_PASSWDMISMATCH";
// static const char *ERR_ALREADYREGISTRED = "462 ERR_ALREADYREGISTRED";

//NICK ERR
static const char *ERR_NONICKNAMEGIVEN  = "431 ERR_NONICKNAMEGIVEN";
// static const char *ERR_ERRONEUSNICKNAME = "432 ERR_ERRONEUSNICKNAME";// not use for now
// static const char *ERR_NICKNAMEINUSE    = "433 ERR_NICKNAMEINUSE";
// static const char *ERR_NICKCOLLISION    = "436 ERR_NICKCOLLISION";// not use for now

//JOIN ERR
// static const char *ERR_NOSUCHCHANNEL  = "403 ERR_NOSUCHCHANNEL";

CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["PART"] = &CommandHandler::_part;
    _cmds["PRVMSG"] = &CommandHandler::_prvmsg;
}

void CommandHandler::execute(Client& client, const Command& cmd) {
    std::map<std::string, CommandHandler::Handler>::iterator it = _cmds.find(cmd.command);

    if (it == _cmds.end()) return; // TODO! if (!h) h = ERR_UNKNOWNCOMMAND 421

    Handler h = (*it).second;
    (this->*h)(client, cmd);
}

void CommandHandler::completeRegistration(Client& client) {
    if (client._hasNick && client._hasUsername && client._hasPassword)
    {
        client.registered = true;
        return client.fillOutBuffer(Reply::welcome(client).c_str(), _server.getEfd());
    }
}
void CommandHandler::_pass(Client& client, const Command& cmd) {
    if (client.registered == true)
        return client.fillOutBuffer(Reply::alreadyRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    if (!cmd.params[0].compare(_server.getPassword()))
        client._hasPassword = true;
    else
        client.fillOutBuffer(Reply::passwdMismatch(client).c_str(), _server.getEfd());
    
    return completeRegistration(client);
}
void CommandHandler::_nick(Client& client, const Command& cmd) {

    if (cmd.params.size() < 1 || cmd.params[0].empty() == true)
        // todo: replace ERR_NONICKNAMEGIVEN with Reply::<good_function>
        return client.fillOutBuffer(ERR_NONICKNAMEGIVEN, _server.getEfd());

    if (_server.getClientByNick(cmd.params[0]) != NULL) 
        return client.fillOutBuffer(Reply::nicknameInUse(client, cmd.params[0]).c_str(), _server.getEfd());

    // TODO: if ( nick has invalid/not compatible characters)
    // fillOutBuffer( ERR_ERRONEUSNICKNAME )
    
    client.setNick(cmd.params[0]);
    std::cout << client.getNick() << std::endl;// console log

    return completeRegistration(client);
}
void CommandHandler::_user(Client& client, const Command& cmd) {
    if (client.registered == true)
        return client.fillOutBuffer(Reply::alreadyRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    client.setUser(cmd.params[0]);

    //TODO: handle realName [USER < username > '' '' < :realName > ]

    return completeRegistration(client);
}
void CommandHandler::_join(Client& client, const Command& cmd) {
    // Create channel if does not already exist.
    // Then add this client to the client* list of this channel
    // and add in client classe the name or anything that can relate to the channel
    // to be able to know in which one he's in.
    
    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    if (cmd.params[0][0] != '#' && cmd.params[0][0] != '&')
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[0]).c_str(), _server.getEfd());

    std::string name = cmd.params[0];

    Channel* channel = _server.getChannelByName(name);
    if (!channel) {
        Channel newChannel(&client, name);
        _server.createNewChannel(&client, name, newChannel);
        channel = _server.getChannelByName(name);
    } else {
        channel->addClient(&client);
        client.channels.push_back(channel);
    }

    for (size_t i = 0; i < channel->_clients.size(); i++)
        channel->_clients[i]->fillOutBuffer(Reply::relayJoin(client, name).c_str(), _server.getEfd());

    if (channel->getTopic().empty()) {}
        // todo! client.fillOutBuffer(Reply::noTopic(client, name).c_str(), _server.getEfd());
    else {}
        // todo! client.fillOutBuffer(Reply::topic(client, name, "Topic").c_str(), _server.getEfd());
    
    std::string names;
    for (size_t i = 0; i < channel->_clients.size(); i++)
        names += channel->_clients[i]->getNick() + " ";

    client.fillOutBuffer(Reply::namReply(client, name, names).c_str(), _server.getEfd());
    client.fillOutBuffer(Reply::endOfNames(client, name).c_str(), _server.getEfd());
}
void CommandHandler::_part(Client& client, const Command& cmd) {
    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    
    Channel* channelToQuit = _server.getChannelByName(cmd.params[0]);
    if (!channelToQuit)
        // TODO
        return client.fillOutBuffer("ERR_NOCHANNEL", _server.getEfd());

    if (channelToQuit->removeClient(&client)) {
        // TODO
        return client.fillOutBuffer("SUCCESS\n", _server.getEfd());
    }

    return client.fillOutBuffer("FAILURE", _server.getEfd());
}
void CommandHandler::_prvmsg(Client& client, const Command& cmd) {
    if (client.channels.empty())
        return;
    else
    {
        std::string tmp = client.getNick() + ": ";
        tmp.append(cmd.params[0]);
        tmp.append("\r\n");
        const char * buff = tmp.c_str();

        size_t i = 0;
        while (i < client.channels[0]->_clients.size())
        {
            client.channels[0]->_clients[i]->fillOutBuffer(buff, _server.getEfd());
            i++;
        }
    }
    static_cast<void>(cmd);

}

CommandHandler::~CommandHandler() {}