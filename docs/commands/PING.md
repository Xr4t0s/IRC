# PING / PONG

## Rôle
Maintenir la connexion vivante et vérifier qu'un pair répond. Les clients IRC
de référence (irssi, HexChat) envoient des `PING` et attendent un `PONG` : sans
réponse correcte, ils coupent la connexion.

## Syntaxe
```
PING <token>
PONG <token>
```

## Préconditions
- Aucune (doit fonctionner même avant enregistrement complet, pour ne pas se
  faire déconnecter pendant le handshake).

## Process — PING reçu (client → serveur)
1. Si `params.size() < 1` → `409 ERR_NOORIGIN` (ou tolérer et renvoyer un token vide).
2. Répondre immédiatement :
   ```
   PONG irc.server.net :<token>
   ```
   où `<token>` est exactement `params[0]` reçu (le renvoyer tel quel).

## Process — PONG reçu (client → serveur)
1. C'est la réponse du client à un `PING` que **le serveur** aurait envoyé.
2. Marquer le client comme « vivant » (reset d'un éventuel timer d'inactivité).
3. Aucune réponse à renvoyer.

## (Optionnel) PING émis par le serveur
- Périodiquement, envoyer `PING :<token>` aux clients silencieux depuis N
  secondes ; s'ils ne répondent pas par `PONG <token>` avant un délai → `QUIT`
  forcé (timeout).

## Réponses
- `PING` → `PONG irc.server.net :<token>`.
- `PONG` → rien.

## Cas limites
- Token avec espaces : c'est un trailing, le renvoyer intégralement.
- `PING` sans token : renvoyer un `PONG` minimal ou `409`.

## Notes d'implémentation
- Réponse construite à la main (pas un numeric) : `PONG` n'a pas de code.
- Utile même sans timeout implémenté : répondre au `PING` du client évite les
  déconnexions intempestives pendant les tests.
