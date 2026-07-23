# PRIVMSG

## Rôle
Envoyer un message à un utilisateur (privé) ou à un channel (diffusé aux membres).

## Syntaxe
```
PRIVMSG <target>{,<target>} :<text>
```
- `<target>` = un nick **ou** un nom de channel (`#`/`&`).

## Préconditions
- Client enregistré (sinon `451`).

## Process (pour **chaque** target)
1. Si aucune cible (`params.size() < 1`) → `411 ERR_NORECIPIENT`, **stop**.
2. Si pas de texte (`params.size() < 2` ou trailing vide) → `412 ERR_NOTEXTTOSEND`, **stop**.
3. **Si la cible est un channel** (`#`/`&`) :
   - Introuvable → `403 ERR_NOSUCHCHANNEL`, **skip**.
   - L'émetteur n'est **pas** membre (et le channel refuse les externes) →
     `404 ERR_CANNOTSENDTOCHAN`, **skip**.
   - Sinon, relayer à **tous les membres SAUF l'émetteur** :
     ```
     :<nick>!<user>@<host> PRIVMSG <channel> :<text>
     ```
4. **Si la cible est un nick** :
   - Introuvable (`_server.getClientByNick`) → `401 ERR_NOSUCHNICK`, **skip**.
   - Sinon, relayer **au seul destinataire** :
     ```
     :<nick>!<user>@<host> PRIVMSG <targetnick> :<text>
     ```

## Réponses
- **Succès** : relais `PRIVMSG` au(x) destinataire(s). **Pas d'écho** à
  l'émetteur, **pas de numeric** de confirmation.
- **Erreurs** : `411`, `412`, `401`, `403`, `404`.

## Broadcast
- Channel : tous les membres **sauf** l'émetteur.
- Nick : uniquement la cible.

## Cas limites
- Cibles multiples séparées par `,` : traiter chacune indépendamment (une erreur
  sur l'une n'empêche pas les autres).
- Texte contenant `:` ou espaces : c'est le trailing, le préserver intégralement.
- S'envoyer un message à soi-même (nick) : autorisé, renvoie le relais à soi.
- Ne jamais dupliquer si l'émetteur apparaît plusieurs fois : l'exclusion
  émetteur suffit côté channel.

## Notes d'implémentation
- Bien distinguer cible channel vs nick sur le **premier caractère**.
- Réutiliser un helper `broadcast(channel, msg, except)` partagé avec `JOIN`,
  `PART`, `KICK`, `TOPIC`, `MODE`.
