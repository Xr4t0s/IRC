# NICK

## Rôle
Définir ou changer le pseudo (nickname). Unique sur tout le serveur.

## Syntaxe
```
NICK <nickname>
```

## Préconditions
- Aucune (peut être envoyée avant enregistrement).
- Un `PASS` valide reste requis pour finaliser l'enregistrement, mais `NICK`
  seul est accepté.

## Format d'un nick valide
- Premier caractère : lettre ou `[]\`_^{|}`.
- Suivants : lettre, chiffre, `-`, ou l'un des spéciaux ci-dessus.
- Longueur bornée (ex. 9 à 30 selon ton choix). Pas d'espace, pas de `:`, pas de `,`.

## Process
1. Si `params.size() < 1 || params[0].empty()` → `431 ERR_NONICKNAMEGIVEN`, **stop**.
2. Si le format est invalide → `432 ERR_ERRONEUSNICKNAME`, **stop**.
3. Si un autre client porte déjà ce nick (`_server.getClientByNick`) → `433
   ERR_NICKNAMEINUSE`, **stop**.
   - Comparaison **insensible à la casse** (IRC : `Bob` == `bob`).
   - Si c'est **le même client** qui redemande son propre nick → no-op silencieux.
4. Mémoriser l'ancien nick.
5. `client.setNick(params[0])` ; `client._hasNick = true`.
6. **Si déjà enregistré** : diffuser le changement (relais) :
   ```
   :<oldnick>!<user>@<host> NICK :<newnick>
   ```
   → à envoyer au client lui-même **et** à tous les membres des channels où il
   est présent (chaque membre **une seule fois**, dé-doublonner).
7. **Sinon** : tenter la complétion d'enregistrement (cf. `PASS.md`).

## Réponses
- **Succès (enregistré)** : relais `NICK` (pas de numeric).
- **Succès (pré-enregistrement)** : silencieux, éventuel `001` si USER+PASS déjà OK.
- **Erreurs** : `431`, `432`, `433`.

## Broadcast
Changement diffusé à l'union des membres de tous ses channels + lui-même.

## Cas limites
- Deux clients demandent le même nick « simultanément » : le premier arrivé
  gagne ; le second reçoit `433`.
- Collision de casse : traiter comme identique.
- Changement de nick alors qu'on est dans plusieurs channels : un seul relais
  par destinataire même s'il partage plusieurs channels.

## Notes d'implémentation
- `_server.getClientByNick(nick)` doit comparer en casefold.
- Prévoir un helper « union des membres de mes channels » réutilisé par `QUIT`.
