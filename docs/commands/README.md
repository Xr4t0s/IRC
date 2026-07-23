# Commandes — index & conventions

Un fichier par commande prise en compte par le sujet ft_irc. Chaque fiche suit
la même structure :

1. **Rôle** — à quoi sert la commande.
2. **Syntaxe** — grammaire (`<>` = requis, `[]` = optionnel, `{}` = répétable).
3. **Préconditions** — état requis (enregistré ? opérateur ? membre ?).
4. **Process** — algorithme étape par étape, dans l'ordre exact de vérification.
5. **Réponses** — succès (relais + numerics) et erreurs (code + nom + condition).
6. **Broadcast** — à qui la commande est diffusée.
7. **Cas limites**.
8. **Notes d'implémentation** — liens avec `Client` / `Channel` / `Server`.

---

## Périmètre (sujet)

| Phase | Commandes |
|-------|-----------|
| Enregistrement | [`PASS`](PASS.md), [`NICK`](NICK.md), [`USER`](USER.md), [`PING`/`PONG`](PING.md), [`QUIT`](QUIT.md) |
| Channels | [`JOIN`](JOIN.md), [`PART`](PART.md), [`PRIVMSG`](PRIVMSG.md), [`NOTICE`](NOTICE.md), [`TOPIC`](TOPIC.md) |
| Opérateurs / modes | [`KICK`](KICK.md), [`INVITE`](INVITE.md), [`MODE`](MODE.md) |

---

## Format des messages

### Ligne reçue (client → serveur)
```
[':' prefix SPACE] command { SPACE param } [SPACE ':' trailing] CRLF
```
Le préfixe entrant est ignoré par un serveur (on fait confiance au fd). Le
parsing produit `struct Command { std::string prefix, command; std::vector<std::string> params; }`.
Voir `docs/PARSING.md`.

### Ligne envoyée (serveur → client)
Deux familles :

- **Réponse numérique** (générée par `Reply::sendReply`) :
  ```
  :irc.server.net <code> <target> :<trailing>\r\n
  ```
  `<target>` = le nick du destinataire (ou `*` s'il n'est pas encore connu).

- **Relais de commande** (echo d'une action à d'autres clients) : la source est
  le **préfixe du client émetteur** :
  ```
  :<nick>!<user>@<host> <COMMAND> <args> [:<trailing>]\r\n
  ```
  Exemple : `:bob!bob@localhost JOIN #42\r\n`

> **Host** : le sujet n'impose rien. Le plus simple est de figer `localhost`
> (ou le nom du serveur). Si tu veux le vrai host, récupère-le au `accept()`.
> Toutes les fiches notent `@localhost` comme placeholder.

### Terminaison
Toute ligne se termine par `\r\n`. Aucune réponse ≠ vide ne doit être envoyée
sans CRLF final.

---

## Préconditions globales

- **`registered`** : un client est enregistré quand `PASS` **puis** `NICK` **et**
  `USER` sont validés. Tant que ce n'est pas le cas, **seules** `PASS`, `NICK`,
  `USER`, `QUIT` (et `PONG`) sont acceptées. Toute autre commande →
  `451 ERR_NOTREGISTERED`.
- **Commande inconnue** → `421 ERR_UNKNOWNCOMMAND`.
- **Paramètres manquants** → `461 ERR_NEEDMOREPARAMS` (jamais 431, qui est
  `ERR_NONICKNAMEGIVEN`, spécifique à `NICK`).

---

## Table des numerics utilisés

| Code | Nom | Usage |
|------|-----|-------|
| 001 | RPL_WELCOME | fin d'enregistrement |
| 002 | RPL_YOURHOST | (optionnel) |
| 003 | RPL_CREATED | (optionnel) |
| 004 | RPL_MYINFO | (optionnel) |
| 331 | RPL_NOTOPIC | `TOPIC`/`JOIN` sans topic défini |
| 332 | RPL_TOPIC | `TOPIC`/`JOIN` avec topic |
| 341 | RPL_INVITING | confirmation à l'inviteur |
| 353 | RPL_NAMREPLY | liste des membres (`JOIN`, `NAMES`) |
| 366 | RPL_ENDOFNAMES | fin de la liste des membres |
| 324 | RPL_CHANNELMODEIS | consultation des modes |
| 401 | ERR_NOSUCHNICK | nick cible introuvable |
| 403 | ERR_NOSUCHCHANNEL | channel introuvable / nom invalide |
| 404 | ERR_CANNOTSENDTOCHAN | envoi refusé sur le channel |
| 405 | ERR_TOOMANYCHANNELS | (optionnel) trop de channels |
| 411 | ERR_NORECIPIENT | `PRIVMSG` sans cible |
| 412 | ERR_NOTEXTTOSEND | `PRIVMSG` sans texte |
| 421 | ERR_UNKNOWNCOMMAND | commande inconnue |
| 431 | ERR_NONICKNAMEGIVEN | `NICK` sans paramètre |
| 432 | ERR_ERRONEUSNICKNAME | nick au format invalide |
| 433 | ERR_NICKNAMEINUSE | nick déjà pris |
| 441 | ERR_USERNOTINCHANNEL | cible pas dans le channel |
| 442 | ERR_NOTONCHANNEL | émetteur pas dans le channel |
| 443 | ERR_USERONCHANNEL | `INVITE` d'un membre déjà présent |
| 451 | ERR_NOTREGISTERED | commande avant enregistrement |
| 461 | ERR_NEEDMOREPARAMS | paramètres manquants |
| 462 | ERR_ALREADYREGISTRED | ré-enregistrement (`PASS`/`USER`) |
| 464 | ERR_PASSWDMISMATCH | mauvais mot de passe |
| 471 | ERR_CHANNELISFULL | `+l` atteint |
| 472 | ERR_UNKNOWNMODE | flag de mode inconnu |
| 473 | ERR_INVITEONLYCHAN | `+i` sans invitation |
| 475 | ERR_BADCHANNELKEY | `+k` mauvaise clé |
| 476 | ERR_BADCHANMASK | nom de channel invalide |
| 482 | ERR_CHANOPRIVSNEEDED | action réservée aux opérateurs |

> Le message exact du trailing est libre (les clients affichent souvent leur
> propre texte selon le code). Garde des libellés cohérents.
