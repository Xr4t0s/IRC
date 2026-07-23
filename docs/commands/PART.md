# PART

## Rôle
Quitter un ou plusieurs channels.

## Syntaxe
```
PART <channel>{,<channel>} [:<reason>]
```

## Préconditions
- Client enregistré (sinon `451`).

## Process (pour **chaque** channel)
1. Si `params.size() < 1` → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Chercher le channel. S'il n'existe pas → `403 ERR_NOSUCHCHANNEL`, **skip**.
3. Si le client **n'est pas** membre → `442 ERR_NOTONCHANNEL`, **skip**.
4. Construire le relais :
   ```
   :<nick>!<user>@<host> PART <channel> [:<reason>]
   ```
5. Diffuser ce relais à **tous** les membres du channel, **y compris l'émetteur**
   (c'est ce qui confirme au client qu'il est bien parti).
6. Retirer le client des membres (`Channel::removeClient`) et retirer le channel
   de `client.channels`.
7. Si le channel devient **vide** → le supprimer de `_server._channels`.
   - Si l'émetteur était le dernier opérateur mais qu'il reste du monde : choix
     d'implémentation (promouvoir un membre ou non ; non exigé).

## Réponses
- **Succès** : relais `PART` (broadcast, émetteur inclus). **Aucun numeric.**
- **Erreurs** : `461`, `403`, `442`.

## Broadcast
Relais `PART` à tous les membres, émetteur inclus, **avant** de le retirer
(sinon il ne recevrait pas sa propre confirmation).

## Cas limites
- Ordre important : diffuser **puis** retirer.
- Raison optionnelle : si absente, ne pas envoyer de trailing (ou un défaut).
- `PART` sur un channel inexistant vs channel existant où on n'est pas : deux
  codes différents (`403` vs `442`).

## Notes d'implémentation
- Remplace la version « SUCCESS/FAILURE » brute : renvoyer un **relais PART**,
  pas un texte maison.
- `Channel::removeClient` doit gérer proprement l'itérateur (ne pas utiliser un
  `const_iterator` avec `erase` en C++98 ; avancer l'itérateur après suppression).
