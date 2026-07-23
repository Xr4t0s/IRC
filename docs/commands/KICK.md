# KICK

## Rôle
Éjecter de force un utilisateur d'un channel. Réservé aux **opérateurs**.

## Syntaxe
```
KICK <channel> <user>{,<user>} [:<comment>]
```

## Préconditions
- Client enregistré.
- Émetteur **membre** du channel et **opérateur**.

## Process (pour **chaque** user ciblé)
1. Si `params.size() < 2` (channel + user) → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Channel introuvable → `403 ERR_NOSUCHCHANNEL`, **stop**.
3. Émetteur **pas membre** du channel → `442 ERR_NOTONCHANNEL`, **stop**.
4. Émetteur **pas opérateur** → `482 ERR_CHANOPRIVSNEEDED`, **stop**.
5. Cible **pas membre** du channel → `441 ERR_USERNOTINCHANNEL`, **skip**.
6. Déterminer le commentaire : trailing s'il existe, sinon le nick de l'émetteur.
7. Diffuser à **tous** les membres (émetteur **et** cible inclus) :
   ```
   :<nick>!<user>@<host> KICK <channel> <targetnick> :<comment>
   ```
8. Retirer la cible des membres (`Channel::removeClient`) et de ses `channels`.
9. Si le channel devient vide → le supprimer.

## Réponses
- **Succès** : relais `KICK` (broadcast). Aucun numeric côté succès.
- **Erreurs** : `461`, `403`, `442`, `482`, `441`.

## Broadcast
Relais `KICK` à tous les membres, **cible incluse** (elle doit voir qu'elle a été
kickée), **avant** de la retirer.

## Cas limites
- Un opérateur peut se kicker lui-même (équivaut à un `PART`).
- Kick de plusieurs users : traiter chacun ; l'un peut réussir, l'autre donner
  `441`.
- Ordre : broadcast **puis** suppression.

## Notes d'implémentation
- Nécessite `isOperator`, `getMember(nick)` sur `Channel`.
- Réutilise le même chemin de suppression que `PART`/`QUIT`.
