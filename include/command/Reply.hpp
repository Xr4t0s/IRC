#pragma once

#include <iostream>
#include <vector>

class Client;

class Reply {
    private:
        static std::string _serializeNumeric(int code, const std::string& target, const std::string& trailing);
        static std::string _serializeBroadcast(int code, const std::string& target, const std::vector<std::string> params, const std::string& trailing);

    public:
        Reply();
        ~Reply();

        // ---- Enregistrement --------------------------------------------------
        static std::string welcome(Client& client);                                             // 001
        static std::string needMoreParams(Client& client, const std::string& command);          // 461
        static std::string alreadyRegistered(Client& client);                                   // 462
        static std::string passwdMismatch(Client& client);                                      // 464
        static std::string notRegistered(Client& client);                                       // 451

        // ---- Nick ------------------------------------------------------------
        static std::string noNicknameGiven(Client& client);                                     // 431
        static std::string erroneusNickname(Client& client, const std::string& badNick);        // 432
        static std::string nicknameInUse(Client& client, const std::string& nick);              // 433

        // ---- Channel : topic / noms / invitation / modes ---------------------
        static std::string noTopic(Client& client, const std::string& channel);                 // 331
        static std::string topic(Client& client, const std::string& channel, const std::string& topic); // 332
        static std::string inviting(Client& client, const std::string& targetNick, const std::string& channel); // 341
        static std::string namReply(Client& client, const std::string& channel, const std::string& names);      // 353
        static std::string endOfNames(Client& client, const std::string& channel);              // 366
        static std::string channelModeIs(Client& client, const std::string& channel, const std::string& modes); // 324

        // ---- Erreurs : cibles / commandes ------------------------------------
        static std::string noSuchNick(Client& client, const std::string& nick);                 // 401
        static std::string noSuchChannel(Client& client, const std::string& channel);           // 403
        static std::string cannotSendToChan(Client& client, const std::string& channel);        // 404
        static std::string tooManyChannels(Client& client, const std::string& channel);         // 405
        static std::string noRecipient(Client& client, const std::string& command);             // 411
        static std::string noTextToSend(Client& client);                                        // 412
        static std::string unknownCommand(Client& client, const std::string& command);          // 421

        // ---- Erreurs : appartenance channel ----------------------------------
        static std::string userNotInChannel(Client& client, const std::string& nick, const std::string& channel); // 441
        static std::string notOnChannel(Client& client, const std::string& channel);            // 442
        static std::string userOnChannel(Client& client, const std::string& nick, const std::string& channel);    // 443

        // ---- Erreurs : modes / accès channel ---------------------------------
        static std::string channelIsFull(Client& client, const std::string& channel);           // 471
        static std::string unknownMode(Client& client, char mode);                              // 472
        static std::string inviteOnlyChan(Client& client, const std::string& channel);          // 473
        static std::string badChannelKey(Client& client, const std::string& channel);           // 475
        static std::string badChanMask(Client& client, const std::string& channel);             // 476
        static std::string chanOprivsNeeded(Client& client, const std::string& channel);        // 482

        // ---- Relais d'action (source = préfixe du client émetteur) -----------
        static std::string relayJoin(Client& src, const std::string& channel);
        static std::string relayPart(Client& src, const std::string& channel, const std::string& reason);
        static std::string relayQuit(Client& src, const std::string& reason);
        static std::string relayNick(Client& src, const std::string& oldNick, const std::string& newNick);
        static std::string relayPrivmsg(Client& src, const std::string& target, const std::string& text);
        static std::string relayNotice(Client& src, const std::string& target, const std::string& text);
        static std::string relayKick(Client& src, const std::string& channel, const std::string& targetNick, const std::string& comment);
        static std::string relayInvite(Client& src, const std::string& targetNick, const std::string& channel);
        static std::string relayTopic(Client& src, const std::string& channel, const std::string& topic);
        static std::string relayMode(Client& src, const std::string& channel, const std::string& changes);

        // ---- Keepalive -------------------------------------------------------
        static std::string pong(const std::string& token);
};
