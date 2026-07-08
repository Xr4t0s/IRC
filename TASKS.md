# TASKS — ft_irc

Liste de tâches partagée. Cochez `[x]` ce qui est fait, laissez `[ ]` sinon.
Regroupé par phase, dans l'ordre logique d'implémentation.

> Légende : `[x]` fait · `[~]` en cours · `[ ]` à faire

---

## Phase 0 — Fondations

- [x] Structure du projet (`include/` + `src/`, dossiers `core/command/utils`)
- [x] Makefile (`-Wall -Wextra -Werror -std=c++98`, build clean)
- [x] Classe `Error` héritant de `std::exception`
- [x] Parsing des arguments `./ircserv <port> <password>` (validation port + pass)

## Phase 1 — Réseau / boucle serveur

- [x] Création socket (`socket`, `SO_REUSEADDR`)
- [x] Sockets non-bloquantes (`O_NONBLOCK`)
- [x] `bind` + `listen`
- [x] Setup epoll (`epoll_create1`, watchlist `EPOLLIN`)
- [x] Boucle `epoll_wait` (lecture EPOLLIN)
- [x] `accept` nouveaux clients + ajout watchlist
- [x] Suppression client (`EPOLL_CTL_DEL`, `close`, erase de la map)
- [x] Gestion signal `SIGINT` (`g_running` en `volatile sig_atomic_t`)
- [ ] Gérer `EINTR` : `epoll_wait` renvoie -1 au Ctrl+C → ne pas traiter comme
      une erreur fatale (sortir proprement via `g_running`)

## Phase 2 — Réception / bufferisation

- [x] `Client` : buffer d'entrée `_buff`, accumulation via `fillBuffer`
- [x] `hasCompleteCommand()` (détection `\r\n`)
- [x] `extractCommand()` (extraction + `erase`, gestion multi-commandes)
- [x] Boucle serveur `while (hasCompleteCommand()) execute(...)`

## Phase 3 — Dispatch des commandes

- [x] `struct Command { prefix, command, vector<string> params }`
- [x] `CommandHandler` : table de dispatch (`map<string, méthode>`)
- [x] `execute()` avec `find()` (pas d'insertion parasite)
- [x] Forward declaration `Server`/`Client` (casser le cycle d'include)
- [~] `parseCommand()` : ligne IRC → `Command` (voir `docs/PARSING.md`)
  - [ ] Extraction préfixe optionnel (`:` en début de ligne)
  - [ ] Extraction commande
  - [ ] Middle params
  - [ ] Trailing (`:` en début de param, espaces préservés)
  - [ ] Cas limites : trailing vide, `:` littéral, ligne vide

## Phase 4 — Chemin d'envoi (réponses)

- [ ] Buffer de sortie par client (`_outBuf`) + `enqueue(reply)`
- [ ] `send` non-bloquant, gestion envoi partiel (compte d'octets, sans `errno`)
- [ ] Activer `EPOLLOUT` seulement quand `_outBuf` non vide (`EPOLL_CTL_MOD`)
- [ ] Repasser en `EPOLLIN` seul quand `_outBuf` vidé (éviter 100% CPU)
- [ ] Inspecter `events[i].events` dans la boucle (IN / OUT / ERR / HUP)
- [ ] Fichier `Replies.hpp` : réponses numériques (format `:serveur CODE ...`)

## Phase 5 — Enregistrement (registration)

- [ ] `PASS` (vérifier mot de passe, `ERR_PASSWDMISMATCH` 464)
- [ ] `NICK` (unicité, format, `ERR_NICKNAMEINUSE` 433, `ERR_ERRONEUSNICKNAME` 432)
- [ ] `USER` (username + realname via trailing)
- [ ] Passage `registered` quand PASS+NICK+USER OK → `RPL_WELCOME` 001
- [ ] Rejet des commandes avant registration (`ERR_NOTREGISTERED` 451)
- [ ] `PING` / `PONG` (garder la connexion vivante)
- [ ] `QUIT` (déconnexion propre, notifier les channels)

## Phase 6 — Channels

- [ ] Classe `Channel` : membres, opérateurs, topic, clé, limite, modes
- [ ] `JOIN` (créer/rejoindre, `RPL_NAMREPLY` 353, `RPL_ENDOFNAMES` 366)
- [ ] `PART` (quitter un channel)
- [ ] `PRIVMSG` vers un channel (broadcast aux membres) et vers un nick
- [ ] `NOTICE`
- [ ] `TOPIC` (voir / définir, `RPL_TOPIC` 332 / `RPL_NOTOPIC` 331)
- [ ] Broadcast propre (exclure l'émetteur quand il faut)

## Phase 7 — Opérateurs & modes

- [ ] `KICK` (éjecter un user, `ERR_CHANOPRIVSNEEDED` 482)
- [ ] `INVITE` (inviter sur un channel)
- [ ] `MODE +i` / `-i` (invite-only)
- [ ] `MODE +t` / `-t` (topic réservé aux ops)
- [ ] `MODE +k` / `-k` (clé du channel)
- [ ] `MODE +o` / `-o` (donner/retirer op)
- [ ] `MODE +l` / `-l` (limite de membres)

## Phase 8 — Robustesse & rendu

- [ ] Tester avec un vrai client IRC de référence (irssi / HexChat)
- [ ] Tester données partielles (paquet coupé au milieu, `nc` avec pipes)
- [ ] Aucune fuite mémoire (destruction propre clients/channels)
- [ ] Nettoyer les `std::cout` de debug
- [ ] Relire le sujet : fonctions autorisées (`docs/ALLOWED_FUNCTIONS.md`)

## Bonus (optionnel)

- [ ] Transfert de fichiers (DCC)
- [ ] Bot

---

## Docs de référence

- `docs/DEV_DOC.md` — architecture générale
- `docs/USER_DOC.md` — comment tester
- `docs/PARSING.md` — parsing des commandes / trailing
- `docs/ALLOWED_FUNCTIONS.md` — fonctions autorisées par le sujet
