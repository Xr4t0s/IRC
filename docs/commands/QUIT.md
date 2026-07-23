# QUIT

## Rôle
Déconnexion propre du client. Notifie tous les channels où il était présent puis
ferme la socket et libère les ressources.

## Syntaxe
```
QUIT [:<reason>]
```

## Préconditions
- Aucune (accepté à tout moment, même non enregistré).

## Process
1. Déterminer la raison : le trailing s'il existe, sinon un défaut
   (`"Client Quit"`).
2. Construire le relais :
   ```
   :<nick>!<user>@<host> QUIT :<reason>
   ```
3. Diffuser ce relais à **l'union** des membres de tous les channels du client,
   **en excluant le client lui-même**, chaque destinataire **une seule fois**.
4. Retirer le client de la liste `_clients` de **chaque** channel où il figurait
   (`Channel::removeClient`). Si un channel devient vide → le supprimer de
   `_server._channels`.
   - S'il était opérateur : voir si le channel doit promouvoir quelqu'un d'autre
     (choix d'implémentation ; pas exigé par le sujet).
5. (Optionnel) Envoyer au client partant : `ERROR :Closing Link: <nick>`.
6. `EPOLL_CTL_DEL` sur son fd, `close(fd)`, `erase` de la map des clients.

## Réponses
- **Aux autres** : relais `QUIT`.
- **Au client** : `ERROR` optionnel, puis fermeture.

## Broadcast
Union des membres de ses channels, émetteur exclu, sans doublon.

## Cas limites
- Déconnexion **brutale** (socket fermée, `recv` == 0 / `EPOLLHUP`) : traiter
  **exactement** comme un `QUIT` (même nettoyage, même broadcast avec une raison
  type `"Connection closed"`). Centraliser dans une fonction `disconnect(client)`.
- Client jamais entré dans un channel : pas de broadcast, juste le nettoyage.
- Ne pas déréférencer le client après `close`/`erase` : faire le broadcast
  **avant** de libérer.

## Notes d'implémentation
- Factoriser `disconnect(Client&, reason)` réutilisée par `QUIT` **et** par la
  détection de déconnexion dans la boucle epoll.
- Attention à l'itération : ne pas modifier `_channels`/`_clients` pendant qu'on
  les parcourt (collecter d'abord, supprimer ensuite).
