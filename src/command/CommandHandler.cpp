#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

CommandHandler::CommandHandler(Server& server) : _server(server) {
    _cmds["PASS"] = &CommandHandler::_pass;
    _cmds["NICK"] = &CommandHandler::_nick;
    _cmds["USER"] = &CommandHandler::_user;
    _cmds["JOIN"] = &CommandHandler::_join;
    _cmds["PART"] = &CommandHandler::_part;
    _cmds["PRIVMSG"] = &CommandHandler::_privmsg;
    _cmds["TOPIC"] = &CommandHandler::_topic;
    _cmds["KICK"] = &CommandHandler::_kick;
    _cmds["QUIT"] = &CommandHandler::_quit;
    _cmds["INVITE"] = &CommandHandler::_invite;

}

void CommandHandler::execute(Client& client, const Command& cmd) {
    std::map<std::string, CommandHandler::Handler>::iterator it = _cmds.find(cmd.command);

    if (it == _cmds.end())
        return client.fillOutBuffer(Reply::unknownCommand(client, cmd.command).c_str(), _server.getEfd());

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
        return client.fillOutBuffer(Reply::noNicknameGiven(client).c_str(), _server.getEfd());

    if (_server.getClientByNick(cmd.params[0]) != NULL && client.getNick() != cmd.params[0])
        return client.fillOutBuffer(Reply::nicknameInUse(client, cmd.params[0]).c_str(), _server.getEfd());

    // TODO: if ( nick has invalid/not compatible characters)
    // fillOutBuffer( ERR_ERRONEUSNICKNAME )
    
    client.setNick(cmd.params[0]);

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
    
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    std::vector<std::string> channels = splitBy(cmd.params[0], ',');
    for (size_t i = 0; i < channels.size(); i++) {
        std::string name = channels[i];

        if (name[0] != '#' && name[0] != '&') {
            client.fillOutBuffer(Reply::noSuchChannel(client, name).c_str(), _server.getEfd());
            continue;
        }

        Channel* channel = _server.getChannelByName(name);
        if (!channel) {
            Channel newChannel(&client, name);
            _server.createNewChannel(&client, name, newChannel);
            channel = _server.getChannelByName(name);
            channel->addOperator(&client);
        } else {
            if (channel->i) {
                if (!channel->isWhitelisted(client.getNick()))
                    return client.fillOutBuffer(Reply::inviteOnlyChan(client, channel->getName()).c_str(), _server.getEfd());
                channel->removeWhitelist(client.getNick());
            }
            if (channel->l != -1 && channel->l < static_cast<int>(channel->_clients.size()) + 1)
                return client.fillOutBuffer(Reply::channelIsFull(client, channel->getName()).c_str(), _server.getEfd());
            //TODO: si y'a un mot de passe
            channel->addClient(&client);
            client.channels.push_back(channel);
        }

        for (size_t i = 0; i < channel->_clients.size(); i++)
            channel->_clients[i]->fillOutBuffer(Reply::relayJoin(client, name).c_str(), _server.getEfd());

        if (channel->getTopic().empty())
            client.fillOutBuffer(Reply::noTopic(client, channel->getName()).c_str(), _server.getEfd());
        else
            client.fillOutBuffer(Reply::topic(client, channel->getName(), channel->getTopic()).c_str(), _server.getEfd());
        
        std::string names;
        for (size_t i = 0; i < channel->_clients.size(); i++)
            names += (channel->isOperator(*channel->_clients[i]) ? "@" : "") + channel->_clients[i]->getNick() + " ";

        client.fillOutBuffer(Reply::namReply(client, name, names).c_str(), _server.getEfd());
        client.fillOutBuffer(Reply::endOfNames(client, name).c_str(), _server.getEfd());
    }
}
void CommandHandler::_part(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    
    std::vector<std::string> channelsToQuit = splitBy(cmd.params[0], ',');
    for (size_t i = 0; i < channelsToQuit.size(); i++) {
        Channel* channelToQuit = _server.getChannelByName(channelsToQuit[i]);
        if (!channelToQuit)
            return client.fillOutBuffer(Reply::noSuchChannel(client, channelsToQuit[i]).c_str(), _server.getEfd());
            
        if (channelToQuit->findClient(client)) {
            for (size_t i = 0; i < channelToQuit->_clients.size(); i++)
                channelToQuit->_clients[i]->fillOutBuffer(Reply::relayPart(client, channelToQuit->getName(), cmd.params[1]).c_str(), _server.getEfd());
            if (channelToQuit->isOperator(client))
                channelToQuit->removeOperator(&client);

            if (channelToQuit->isWhitelisted(client.getNick()))
                channelToQuit->removeWhitelist(client.getNick());

            channelToQuit->removeClient(&client);
            client.removeChannel(channelToQuit);
            if (channelToQuit->_clients.empty())
                _server.deleteChannel(channelToQuit);
            
        } else
            return client.fillOutBuffer(Reply::notOnChannel(client, channelToQuit->getName()).c_str(), _server.getEfd());
    }

}
void CommandHandler::_privmsg(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        client.fillOutBuffer(Reply::noRecipient(client, cmd.command).c_str(), _server.getEfd());
    if (cmd.params.size() < 2 || cmd.params[1].empty())
        client.fillOutBuffer(Reply::noTextToSend(client).c_str(), _server.getEfd());
    
    std::vector<std::string> tmp = splitBy(cmd.params[0], ',');
    std::vector<std::string>::iterator it = tmp.begin();
    while (it != tmp.end())
    {
        if ((*it)[0] == '&' || (*it)[0] == '#')
        {
            Channel * channel = _server.getChannelByName((*it));

            if (channel == NULL)
                return client.fillOutBuffer(Reply::noSuchChannel(client, *it).c_str(), _server.getEfd());
            
            if (channel->findClient(client) == NULL)
                return client.fillOutBuffer(Reply::cannotSendToChan(client, *it).c_str(), _server.getEfd());

            for (size_t i = 0; i < channel->_clients.size(); i++)
            {
                if (client.getNick() != channel->_clients[i]->getNick())
                    channel->_clients[i]->fillOutBuffer(Reply::relayPrivmsg(client, *it, cmd.params[1]).c_str(), _server.getEfd());
            }
        }
        else
        {
            Client * target = _server.getClientByNick(*it);
            if (target == NULL)
                return client.fillOutBuffer(Reply::noSuchNick(client, *it).c_str(), _server.getEfd());
            target->fillOutBuffer(Reply::relayPrivmsg(client, (*it), cmd.params[1]).c_str(), _server.getEfd()); 
        }
        it++;
    }
}

void CommandHandler::_topic(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    Channel * channel = _server.getChannelByName(cmd.params[0]);
    if (channel == NULL)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[0]).c_str(), _server.getEfd());
    if (channel->findClient(client) == NULL)
        return client.fillOutBuffer(Reply::notOnChannel(client, channel->getName()).c_str(), _server.getEfd());
    if (cmd.params.size() < 2) {
        if (channel->getTopic().empty())
            return client.fillOutBuffer(Reply::noTopic(client, channel->getName()).c_str(), _server.getEfd());
        else
            return client.fillOutBuffer(Reply::topic(client, channel->getName(), channel->getTopic()).c_str(), _server.getEfd());
    }
    if (channel->t) {
        if (!(channel->isOperator(client)))
            return client.fillOutBuffer(Reply::chanOprivsNeeded(client, channel->getName()).c_str(), _server.getEfd());
    }
    channel->setTopic(cmd.params[1]);
    for (size_t i = 0; i < channel->_clients.size(); i++)
        channel->_clients[i]->fillOutBuffer(Reply::relayTopic(client, channel->getName(), cmd.params[1]).c_str(), _server.getEfd());
}

void CommandHandler::_kick(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());


    if(cmd.params.size() < 2)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    std::string comment = (cmd.params.size() == 3 ? cmd.params[2] : client.getNick());

    std::vector<std::string> channels = splitBy(cmd.params[0], ',');
    std::vector<std::string> users = splitBy(cmd.params[1], ',');

    for (size_t i = 0; i < channels.size(); i++)
    {
        Channel* channel = _server.getChannelByName(channels[i]);

        if(!channel)
            return client.fillOutBuffer(Reply::noSuchChannel(client, channels[i]).c_str(), _server.getEfd());
        
        if (!channel->isOperator(client))
                return client.fillOutBuffer(Reply::chanOprivsNeeded(client, channels[i]).c_str(), _server.getEfd());

        if (!channel->findClient(client))
            return client.fillOutBuffer(Reply::notOnChannel(client, channels[i]).c_str(), _server.getEfd());

        Client * target = _server.getClientByNick(users[i]);
        if (!target || !channel->findClient(*target)) {
            client.fillOutBuffer(Reply::userNotInChannel(client, users[i],channels[i]).c_str(), _server.getEfd());
            continue;
        }
        for (size_t y = 0; y < channel->_clients.size(); y++)
            channel->_clients[y]->fillOutBuffer(Reply::relayKick(client, channels[i], users[i], comment).c_str(), _server.getEfd());
        channel->removeClient(target);
        target->removeChannel(channel);
    }
}

void CommandHandler::_quit(Client& client, const Command& cmd) {
    std::map<std::string, Client*> list;
    std::string trail;

    if (cmd.params.size() < 1)
        trail = "Client quit";
    for (size_t i = 0; i < client.channels.size(); i++)
    {
        for (size_t y = 0; y < client.channels[i]->_clients.size(); y++)
        {
            Client * tmp = client.channels[i]->_clients[y];
            if (client.getNick() != tmp->getNick())
            {
                
                if(list.find(tmp->getNick()) == list.end())
                    list.insert(std::make_pair(tmp->getNick(), tmp));
            }
        }
        if (client.channels[i]->_clients.empty())
            _server.deleteChannel(client.channels[i]);

    }

    std::map<std::string, Client*>::iterator it = list.begin();
    std::map<std::string, Client*>::iterator ite = list.end();

    while (it != ite)
    {
        it->second->fillOutBuffer(Reply::relayQuit(client, (trail.empty() ? cmd.params[0] : trail)).c_str(), _server.getEfd());
        it++;
    }

    _server.remove_client(client);
}

void    CommandHandler::_invite(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());

    if (cmd.params.size() < 2)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());
    
    Client* invited = _server.getClientByNick(cmd.params[0]);
    if (!invited)
        return client.fillOutBuffer(Reply::noSuchNick(client, cmd.params[0]).c_str(), _server.getEfd());
    
    Channel* targetChannel = _server.getChannelByName(cmd.params[1]);
    if (!targetChannel)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[1]).c_str(), _server.getEfd());
    
    if (!targetChannel->findClient(client))
        return client.fillOutBuffer(Reply::notOnChannel(client, cmd.params[1]).c_str(), _server.getEfd());
    
    if (targetChannel->i) {
        if (!targetChannel->isOperator(client))
            return client.fillOutBuffer(Reply::chanOprivsNeeded(client, cmd.params[1]).c_str(), _server.getEfd());
    }

    if (targetChannel->findClient(*invited))
        return client.fillOutBuffer(Reply::userOnChannel(client, cmd.params[0], cmd.params[1]).c_str(), _server.getEfd());

    targetChannel->addWhitelist(invited->getNick());
    
    client.fillOutBuffer(Reply::inviting(client, invited->getNick(), targetChannel->getName()).c_str(), _server.getEfd());
    return invited->fillOutBuffer(Reply::relayInvite(client, invited->getNick(), targetChannel->getName()).c_str(), _server.getEfd());
}

CommandHandler::~CommandHandler() {}