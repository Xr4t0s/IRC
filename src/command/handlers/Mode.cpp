#include "command/CommandHandler.hpp"
#include "core/Server.hpp"
#include "command/Reply.hpp"
#include "utils/Utils.hpp"

void    CommandHandler::_mode(Client& client, const Command& cmd) {
    if (!client.registered)
        return client.fillOutBuffer(Reply::notRegistered(client).c_str(), _server.getEfd());
    
    std::size_t nbParams = cmd.params.size();

    if (nbParams < 1)
        return client.fillOutBuffer(Reply::needMoreParams(client, cmd.command).c_str(), _server.getEfd());

    Channel* targetChannel = _server.getChannelByName(cmd.params[0]);
    if (!targetChannel)
        return client.fillOutBuffer(Reply::noSuchChannel(client, cmd.params[0]).c_str(), _server.getEfd());
    
    if (nbParams == 1)
        return client.fillOutBuffer(Reply::channelModeIs(client, cmd.params[0], serializeMode(targetChannel, client)).c_str(), _server.getEfd());

    if (!targetChannel->isOperator(client))
        return client.fillOutBuffer(Reply::chanOprivsNeeded(client, targetChannel->getName()).c_str(), _server.getEfd());

    std::string modes = cmd.params[1];
    bool        mode = true;
    std::size_t consumed = 0;

    std::string changes;
    std::string changesParams = " ";

    for (size_t i = 0; i < modes.size(); i++) {
        switch (modes[i]) {
            case '+':
                changes += '+';
                mode = true;
                break;

            case '-':
                changes += '-';
                mode = false;
                break;

            case 'i':
                changes += 'i';
                targetChannel->i = mode;
                break;

            case 't':
                changes += 't';
                targetChannel->t = mode;
                break;

            case 'k':
                changes += 'k';
                if (mode) {
                    if (2 + consumed >= nbParams)
                        return client.fillOutBuffer(
                            Reply::needMoreParams(client, cmd.command).c_str(),
                            _server.getEfd()
                        );

                    targetChannel->k = cmd.params[2 + consumed];
                    changesParams += targetChannel->k + " ";
                    ++consumed;
                } else {
					if (!targetChannel->k.empty())
						changesParams += targetChannel->k + " ";
					else
						changes.erase(--changes.end());
                    targetChannel->k.clear();
                }

                break;

            case 'l':
                changes += 'l';
                if (mode) {
                    if (2 + consumed >= nbParams)
                        return client.fillOutBuffer(
                            Reply::needMoreParams(client, cmd.command).c_str(),
                            _server.getEfd()
                        );

                    int limit = stringToInt(cmd.params[2 + consumed]);
                    ++consumed;

                    if (limit <= 0)
                        break;
                    
                    targetChannel->l = limit;
                    changesParams += intToString(targetChannel->l) + " ";
                } else
                    targetChannel->l = -1;
                
                break;

            case 'o': {
                changes += 'o';
                if (2 + consumed >= nbParams)
                    return client.fillOutBuffer(
                        Reply::needMoreParams(client, cmd.command).c_str(),
                        _server.getEfd()
                    );

                Client* target = _server.getClientByNick(cmd.params[2 + consumed]);
                ++consumed;

                if (!target)
                    return client.fillOutBuffer(
                        Reply::noSuchNick(client, cmd.params[2 + consumed - 1]).c_str(),
                        _server.getEfd()
                    );

                if (!targetChannel->findClient(*target))
                    return client.fillOutBuffer(
                        Reply::userNotInChannel(client, target->getNick(), targetChannel->getName()).c_str(),
                        _server.getEfd()
                    );

                if (mode)
                    targetChannel->addOperator(target);
                else
                    targetChannel->removeOperator(target);
                    
                changesParams += target->getNick() + " ";

                break;
            }

            default:
                client.fillOutBuffer(Reply::unknownMode(client, modes[i]).c_str(), _server.getEfd());
                break;
        }
    }

    if (nbParams > 1) {
        if (changes.size() == 1)
            changes.erase(--changes.end());
        std::string channelName = targetChannel->getName();
        for (size_t i = 0; i < targetChannel->clients.size(); i++) {
            Client* clientIndex = targetChannel->clients[i];
            clientIndex->fillOutBuffer(Reply::relayMode(client, channelName, changes + changesParams).c_str(), _server.getEfd());
        }
    }
    return ;
}