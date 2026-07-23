# USER

## Rôle
Déclarer le username et le realname du client. Troisième et dernière étape de
l'enregistrement.

## Syntaxe
```
USER <username> <hostname> <servername> <realname>
```
- `<realname>` est le **trailing** (précédé de `:`, peut contenir des espaces).
- `<hostname>` et `<servername>` sont ignorés par un serveur moderne (présents
  pour raisons historiques) mais **doivent** être présents pour un parsing valide.

## Préconditions
- Client **non** encore enregistré.

## Process
1. Si `registered == true` → `462 ERR_ALREADYREGISTRED`, **stop**.
2. Si `params.size() < 4` (username, hostname, servername, realname) →
   `461 ERR_NEEDMOREPARAMS`, **stop**.
   - Tolérance possible : accepter `< 1` comme minimal si tu ne veux valider que
     le username, mais rester cohérent avec le parsing du trailing.
3. `client.setUser(params[0])` ; stocker le realname (`params[3]` ou le trailing).
4. `client._hasUsername = true`.
5. Tenter la complétion d'enregistrement (cf. `PASS.md`).

## Réponses
- **Succès** : silencieux, ou `001 RPL_WELCOME` si `PASS`+`NICK` déjà validés.
- **Erreurs** : `461`, `462`.

## Cas limites
- `USER` après enregistrement → `462` (on ne change pas d'identité).
- Realname vide (`:` seul) : accepté.
- Realname avec espaces : géré uniquement si le trailing est correctement parsé.

## Notes d'implémentation
- Membres : `client._user`, realname (à ajouter si tu veux l'exposer dans le
  préfixe `WHOIS`/relais), `client._hasUsername`.
- Le username sert dans le préfixe des relais : `:<nick>!<user>@<host>`.
