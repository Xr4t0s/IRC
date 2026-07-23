# NOTICE

## Rôle
Identique à `PRIVMSG` fonctionnellement, mais **ne génère JAMAIS de réponse
automatique** (ni numeric d'erreur, ni écho). Utilisée par les clients/bots pour
éviter les boucles de réponses.

## Syntaxe
```
NOTICE <target> :<text>
```

## Préconditions
- Client enregistré.

## Process
Comme `PRIVMSG`, **mais** :
1. Si cible manquante ou texte vide → **ne rien renvoyer** (pas de `411`/`412`).
2. Cible channel introuvable / nick introuvable / envoi refusé → **silence
   total** (pas de `401`/`403`/`404`).
3. Si la cible est valide, relayer :
   ```
   :<nick>!<user>@<host> NOTICE <target> :<text>
   ```

## Réponses
- **Succès** : relais `NOTICE`.
- **Erreurs** : **aucune** — c'est la règle qui distingue `NOTICE` de `PRIVMSG`.

## Broadcast
- Channel : tous les membres sauf l'émetteur.
- Nick : la cible uniquement.

## Cas limites
- Toute condition qui, en `PRIVMSG`, produirait une erreur → en `NOTICE`,
  produit un **abandon silencieux**.

## Notes d'implémentation
- Réutiliser la logique de `PRIVMSG` avec un flag `silent = true` qui court-circuite
  tout envoi de numeric.
- Optionnelle au sens strict du sujet, mais triviale une fois `PRIVMSG` fait, et
  attendue par les clients de référence.
