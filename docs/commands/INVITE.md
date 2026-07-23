# INVITE

## Rôle
Inviter un utilisateur sur un channel. Nécessaire pour rejoindre un channel en
mode `+i` (invite-only).

## Syntaxe
```
INVITE <nick> <channel>
```

## Préconditions
- Client enregistré.
- Émetteur **membre** du channel.
- Si le channel est `+i` : l'émetteur doit être **opérateur** (sinon un non-op ne
  peut pas contourner l'invite-only).

## Process
1. Si `params.size() < 2` → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Nick cible introuvable (`getClientByNick`) → `401 ERR_NOSUCHNICK`, **stop**.
3. Channel introuvable → `403 ERR_NOSUCHCHANNEL`, **stop**.
   - (Certaines implémentations autorisent l'invitation vers un channel non encore
     créé ; le plus simple est d'exiger qu'il existe.)
4. Émetteur **pas membre** → `442 ERR_NOTONCHANNEL`, **stop**.
5. Si `+i` et émetteur **pas opérateur** → `482 ERR_CHANOPRIVSNEEDED`, **stop**.
6. Cible **déjà membre** → `443 ERR_USERONCHANNEL`, **stop**.
7. Ajouter la cible à la **liste d'invités** du channel (permet de bypasser `+i`
   au prochain `JOIN`).
8. Confirmer à l'émetteur :
   ```
   341 RPL_INVITING <nick> <targetnick> <channel>
   ```
9. Notifier la cible :
   ```
   :<nick>!<user>@<host> INVITE <targetnick> :<channel>
   ```

## Réponses
- **Succès** : `341` à l'inviteur + relais `INVITE` à l'invité.
- **Erreurs** : `461`, `401`, `403`, `442`, `482`, `443`.

## Broadcast
Uniquement l'inviteur (`341`) et l'invité (relais). Pas de diffusion au channel.

## Cas limites
- L'invitation reste valide jusqu'au `JOIN` (elle est **consommée** au join) ou
  jusqu'à ce que la cible quitte / soit kickée.
- Inviter quelqu'un sur un channel non `+i` : autorisé, ça ajoute juste à la
  liste d'invités (sans effet pratique tant que `+i` est off).

## Notes d'implémentation
- `Channel` doit stocker une liste d'invités (`std::set<std::string>` de nicks,
  ou de `Client*`). Nettoyer au `JOIN` réussi et au départ.
- Cohérence avec `JOIN` (`+i`) : le check `473 ERR_INVITEONLYCHAN` consulte cette
  même liste.
