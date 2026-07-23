# PASS

## Rôle
Fournir le mot de passe de connexion au serveur. **Doit** être envoyée avant
`NICK`/`USER` pour compléter l'enregistrement.

## Syntaxe
```
PASS <password>
```

## Préconditions
- Client **non** encore enregistré (`registered == false`).

## Process
1. Si `registered == true` → `462 ERR_ALREADYREGISTRED`, **stop**.
2. Si `params.size() < 1` → `461 ERR_NEEDMOREPARAMS`, **stop**.
3. Comparer `params[0]` au mot de passe du serveur (`_server.getPassword()`).
   - **Match** → `client._hasPassword = true`.
   - **Mismatch** → `464 ERR_PASSWDMISMATCH`. (Choix d'implémentation : soit
     laisser `_hasPassword = false` et bloquer la fin d'enregistrement, soit
     refuser d'aller plus loin. Le sujet veut que sans bon mot de passe on ne
     s'enregistre pas.)
4. Tenter la **complétion d'enregistrement** (voir plus bas).

## Réponses
- **Succès** : aucune réponse directe (silencieux). Le `001` n'est envoyé que
  lorsque `PASS` + `NICK` + `USER` sont tous validés.
- **Erreurs** : `461`, `462`, `464`.

## Complétion d'enregistrement (partagée avec NICK / USER)
```
si (_hasPassword && _hasNick && _hasUsername) et !registered :
    registered = true
    envoyer 001 RPL_WELCOME  ":irc.server.net 001 <nick> :Welcome to our IRC server <nick>!<user>@<host>"
    (optionnel : 002, 003, 004)
```

## Cas limites
- `PASS` envoyé plusieurs fois avant enregistrement : accepté, le dernier
  écrase. Après enregistrement : `462`.
- Mot de passe avec espaces : impossible (un seul token, pas de trailing requis).

## Notes d'implémentation
- Membres concernés : `client._hasPassword`, `client.registered`.
- Ne jamais logger le mot de passe en clair.
