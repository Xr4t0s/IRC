# TOPIC

## Rôle
Consulter ou modifier le sujet (topic) d'un channel.

## Syntaxe
```
TOPIC <channel> [:<topic>]
```
- Sans `<topic>` → **consultation**.
- Avec `<topic>` → **définition** (un trailing vide `:` **efface** le topic).

## Préconditions
- Client enregistré.
- Pour **définir** : être membre du channel ; si `+t` actif, être **opérateur**.

## Process
1. Si `params.size() < 1` → `461 ERR_NEEDMOREPARAMS`, **stop**.
2. Channel introuvable → `403 ERR_NOSUCHCHANNEL`, **stop**.
3. Si l'émetteur **n'est pas** membre → `442 ERR_NOTONCHANNEL`, **stop**.
4. **Consultation** (pas de 2e paramètre) :
   - Topic défini → `332 RPL_TOPIC <nick> <channel> :<topic>`.
   - Sinon → `331 RPL_NOTOPIC <nick> <channel> :No topic is set`.
   - **stop**.
5. **Définition** (2e paramètre présent) :
   - Si `+t` est actif et que l'émetteur **n'est pas opérateur** →
     `482 ERR_CHANOPRIVSNEEDED`, **stop**.
   - Mettre à jour le topic du channel (trailing vide = topic effacé).
   - Diffuser à **tous** les membres (émetteur inclus) :
     ```
     :<nick>!<user>@<host> TOPIC <channel> :<topic>
     ```

## Réponses
- **Consultation** : `332` ou `331`.
- **Définition** : relais `TOPIC` (broadcast).
- **Erreurs** : `461`, `403`, `442`, `482`.

## Broadcast
Relais `TOPIC` à tous les membres (émetteur inclus) lors d'une définition.

## Cas limites
- Distinguer « pas de 2e param » (consultation) de « 2e param vide » (effacement) :
  se baser sur la **présence** du trailing dans la ligne parsée, pas sur sa
  longueur. Un parseur qui perd cette info rend l'effacement impossible.
- `+t` par défaut : à toi de décider si un channel neuf est `+t` ou non (souvent
  non).

## Notes d'implémentation
- `Channel` doit stocker `_topic` (string) + savoir s'il est « défini ».
- Nécessite `Channel::isOperator(Client*)` et le flag de mode `t`.
