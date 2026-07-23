# JOIN

## Rôle
Rejoindre un channel (le créer s'il n'existe pas). Le créateur devient
**opérateur**.

## Syntaxe
```
JOIN <channel>{,<channel>} [<key>{,<key>}]
```
- Plusieurs channels séparés par `,`, avec des clés positionnelles optionnelles.
- Un nom de channel commence par `#` (ou `&`).

## Préconditions
- Client enregistré (sinon `451`).

## Process (pour **chaque** channel de la liste)
1. Si `params.size() < 1` → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Valider le nom : doit commencer par `#`/`&`, sans espace ni `,` ni `:` ni
   `\a`. Sinon → `403 ERR_NOSUCHCHANNEL` (ou `476 ERR_BADCHANMASK`).
3. Chercher le channel (`_server.getChannelByName`).
4. **S'il n'existe pas** :
   - Le créer, ajouter le client à ses membres.
   - Marquer le client **opérateur** de ce channel.
5. **S'il existe** — vérifier les restrictions d'accès **dans cet ordre** :
   - `+k` (clé) : si une clé est définie et que la clé fournie ne correspond pas
     → `475 ERR_BADCHANNELKEY`, **skip ce channel**.
   - `+i` (invite-only) : si actif et que le client n'est **pas** dans la liste
     d'invités → `473 ERR_INVITEONLYCHAN`, **skip**.
   - `+l` (limite) : si `membres.size() >= limite` → `471 ERR_CHANNELISFULL`, **skip**.
   - Sinon : ajouter le client aux membres. Le retirer de la liste d'invités
     (invitation consommée).
6. Enregistrer le channel dans `client.channels`.
7. **Diffuser l'arrivée** — relais à **tous** les membres (le nouveau inclus) :
   ```
   :<nick>!<user>@<host> JOIN <channel>
   ```
8. Envoyer **au nouveau membre** :
   - Le topic : `332 RPL_TOPIC <nick> <channel> :<topic>` s'il existe, sinon
     `331 RPL_NOTOPIC <nick> <channel> :No topic is set`.
   - La liste des noms : `353 RPL_NAMREPLY <nick> = <channel> :<liste>`
     où chaque nick opérateur est préfixé de `@`.
   - Fin : `366 RPL_ENDOFNAMES <nick> <channel> :End of /NAMES list`.

## Réponses
- **Succès** : relais `JOIN` (broadcast) + `332`/`331` + `353` + `366` (au joiner).
- **Erreurs** : `461`, `403`/`476`, `473`, `475`, `471`, (`405` si tu limites le
  nombre de channels par client).

## Broadcast
Relais `JOIN` à tous les membres du channel. `332/353/366` uniquement au joiner.

## Cas limites
- `JOIN 0` : convention IRC = quitter **tous** les channels (équivaut à `PART`
  partout). Optionnel.
- Client déjà membre : no-op (ne pas ré-ajouter, ne pas re-broadcaster).
- Channel créé puis quitté par tous → supprimé ; un nouveau `JOIN` le recrée et
  le joiner redevient opérateur.
- Clé `<key>` positionnelle : la i-ème clé s'applique au i-ème channel.

## Notes d'implémentation
- Le préfixe `@` dans `353` distingue les opérateurs → `Channel` doit exposer un
  test `isOperator(Client*)`.
- Le `353` cible **le nick du joiner** comme `<target>`, avec `= <channel>`
  collé (channel public). Cf. `Reply::sendReply(353, nick + " = " + chan, names)`.
- Toujours faire converger « création » et « rejoint » vers le **même** bloc de
  réponses pour éviter les divergences.
