# MODE

## Rôle
Consulter ou modifier les modes d'un channel. Le sujet exige les cinq modes
suivants : **i, t, k, o, l**. La modification est réservée aux **opérateurs**.

## Syntaxe
```
MODE <channel> [<modestring> [<mode-args>]]
```
- `<modestring>` = suite de `+`/`-` suivis de lettres, ex. `+it-k`, `+o`, `-l`.
- Certains modes prennent un argument (voir tableau).

## Modes gérés
| Flag | Arg (au `+`) | Arg (au `-`) | Effet |
|------|--------------|--------------|-------|
| `i` | — | — | invite-only : `JOIN` interdit sans invitation |
| `t` | — | — | topic verrouillé : seuls les ops changent le `TOPIC` |
| `k` | `<key>` | — | clé (mot de passe) du channel |
| `o` | `<nick>` | `<nick>` | donne/retire le statut opérateur |
| `l` | `<limit>` | — | limite de membres |

## Préconditions
- Client enregistré.
- Pour **modifier** : membre **et** opérateur du channel.

## Process
1. Si `params.size() < 1` → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Channel introuvable → `403 ERR_NOSUCHCHANNEL`, **stop**.
3. **Consultation** (pas de `<modestring>`) :
   - Renvoyer `324 RPL_CHANNELMODEIS <nick> <channel> <modes> [<params>]`
     (ex. `+itl 10`). **stop**.
4. **Modification** :
   - Émetteur **pas opérateur** → `482 ERR_CHANOPRIVSNEEDED`, **stop**.
   - Parcourir `<modestring>` caractère par caractère en gardant le **signe**
     courant (`+` ou `-`) :
     - Consommer un argument dans `<mode-args>` **si nécessaire** (voir tableau ;
       l'ordre des args suit l'ordre des flags qui en réclament).
     - Appliquer :
       - `i` : activer/désactiver `inviteOnly`.
       - `t` : activer/désactiver `topicLock`.
       - `k` : `+k <key>` définit la clé ; `-k` l'efface. (`+k` sur une clé déjà
         posée : remplacer, ou `467 ERR_KEYSET` selon ton choix.)
       - `o` : `+o <nick>`/`-o <nick>` : la cible doit être **membre** sinon
         `441 ERR_USERNOTINCHANNEL` ; ajoute/retire des opérateurs.
       - `l` : `+l <n>` définit la limite (entier > 0) ; `-l` la retire.
     - Flag inconnu → `472 ERR_UNKNOWNMODE <char>` (continuer les autres flags).
     - Argument manquant pour un flag qui en exige un → ignorer ce flag (ou `461`).
   - Diffuser les changements **effectivement appliqués** à tous les membres :
     ```
     :<nick>!<user>@<host> MODE <channel> <changes> [<args>]
     ```
     (recomposer un `<changes>` propre, ex. `+o bob`, `+kl secret 20`).

## Réponses
- **Consultation** : `324`.
- **Modification** : relais `MODE` (broadcast des changements appliqués).
- **Erreurs** : `461`, `403`, `482`, `472`, `441` (pour `o`), éventuellement `467`.

## Broadcast
Relais `MODE` à tous les membres du channel.

## Cas limites
- `+k` puis divulgation : ne pas afficher la clé dans le `324` à un non-membre
  (choix ; souvent affichée aux membres seulement).
- `+l 0` ou valeur non numérique : rejeter (ignorer le flag).
- Regrouper plusieurs changements en une seule ligne de relais.
- Retirer `-o` de soi-même : autorisé (on peut perdre son statut op).
- Ne diffuser que ce qui a **réellement** changé (pas les no-ops).

## Notes d'implémentation
- `Channel` doit porter : `bool inviteOnly, topicLock; std::string key; bool hasKey;
  size_t limit; bool hasLimit; std::set<Client*> operators; std::set<...> invited;`.
- Prévoir `setOperator/unsetOperator`, `isOperator`. Parser `<modestring>` avec un
  index d'args séparé.
- Le premier membre à créer le channel (via `JOIN`) est opérateur initial.
