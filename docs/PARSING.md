# Parsing d'une commande IRC

> Réf : RFC 1459 (https://www.rfc-editor.org/rfc/rfc1459.html) et RFC 2812.

Ce document explique comment découper une ligne IRC en une structure
`Command`, en insistant sur le **trailing** — la partie la plus piégeuse.

---

## 1. La forme d'une ligne

Après avoir retiré le `\r\n` (voir `Client::extractCommand`), une ligne a
cette forme :

```
[:prefixe]  COMMANDE  param1  param2  ...  [:trailing]
```

- **prefixe** : optionnel, présent seulement si la ligne commence par `:`.
  C'est l'expéditeur (`:nick!user@host`). Un client l'envoie rarement, mais on
  doit le tolérer sans planter.
- **COMMANDE** : `PASS`, `NICK`, `JOIN`, `PRIVMSG`...
- **middle params** : séparés par des espaces, sans espace interne.
- **trailing** : le **dernier** paramètre, introduit par `:`. Seul paramètre
  autorisé à **contenir des espaces**.

Notre structure de sortie :

```cpp
struct Command {
    std::string              prefix;
    std::string              command;
    std::vector<std::string> params;   // le trailing = dernier élément
};
```

---

## 2. La règle d'or du trailing

> Un `:` ne marque le début du trailing **que s'il est au DÉBUT d'un
> paramètre** (donc précédé d'un espace). À partir de ce `:`, **tout le reste
> de la ligne = un seul paramètre**, espaces compris, et on **arrête** de
> découper.

Raison d'être : permettre d'envoyer un texte avec des espaces comme **un seul**
paramètre.

```
PRIVMSG #chan :salut ca va
```
→ `params = ["#chan", "salut ca va"]`  (2 params, pas 4)

---

## 3. Les cas limites (à connaître par cœur)

### 3.1 — Un `:` au milieu d'un mot est LITTÉRAL
```
MODE #chan +o user:name
```
Le `:` de `user:name` n'est pas en début de param → pas un trailing.
`user:name` reste un middle param normal. **C'est la position qui compte, pas
le caractère.**

### 3.2 — Le trailing peut contenir des `:`
```
PRIVMSG bob :rdv a 3:30 ou 4:00 ?
```
→ trailing = `"rdv a 3:30 ou 4:00 ?"`. Seul le **premier** `:` (en début de
param) est le marqueur ; les suivants sont littéraux. Idem pour les URLs :
`:http://truc` → `"http://truc"`.

### 3.3 — Le trailing peut être VIDE (piège classique)
```
TOPIC #chan :        → params = ["#chan", ""]   (trailing présent mais vide)
TOPIC #chan          → params = ["#chan"]        (pas de trailing du tout)
```
Sémantique différente : le premier **efface** le topic, le second **interroge**
le topic. Dès qu'on voit un `:` en début de param, on crée un paramètre —
**même s'il ne reste rien après**.

### 3.4 — Les espaces DANS le trailing sont préservés tels quels
```
PRIVMSG #chan :  salut
```
→ trailing = `"  salut"` (2 espaces de tête conservés). Décomposition :
`<espace>` (délimiteur) + `:` (marqueur) + `  salut` (verbatim). **On ne trim
jamais** ce qui suit le `:`.

### 3.5 — Ne pas confondre avec le `:` du PRÉFIXE (début de ligne)
```
:nick!user@host PRIVMSG #chan :coucou
```
Le **premier** `:`, en tout début de ligne, est le **préfixe**, pas un trailing.
On le traite **en premier** (lire jusqu'au 1er espace), puis on parse le reste.
Deux `:`, deux rôles, selon la position.

### 3.6 — Un seul trailing, toujours en dernier
Au maximum **un** trailing par ligne, et **rien** après. Dès qu'on l'a trouvé,
on **stoppe** le découpage.

### 3.7 — Espaces multiples entre middle params
La RFC dit « un espace », mais on **tolère** plusieurs espaces entre les middles
(robustesse). ⚠️ Cette tolérance **s'arrête** au trailing : espaces entre
middles = à ignorer ; espaces **dans** le trailing = sacrés.

### 3.8 — Ligne vide ou commande seule
- Ligne vide (juste `\r\n`) : pas de commande → ignorer proprement, pas de crash.
- Commande sans params (`LIST`) : `command = "LIST"`, `params` vide. Normal.

---

## 4. L'algorithme (en mots)

1. Si la ligne commence par `:` → extraire le **préfixe** (jusqu'au 1er espace),
   l'enlever.
2. Extraire la **commande** (jusqu'au prochain espace).
3. Boucler sur les params. À chaque itération, **avant** de lire le token,
   tester s'il **commence par `:`** :
   - **Oui** → tout le reste de la ligne (sans le `:`) = **trailing**, on
     l'ajoute et on **stoppe**.
   - **Non** → middle param normal (jusqu'au prochain espace), on continue.

Le test « ce token commence-t-il par `:` ? » à chaque tour est le **cœur** du
parser. Tout le reste en découle.

---

## 5. Les deux pièges qui font échouer les soutenances

1. **Le trailing vide** (§3.3) : confondre « trailing vide » et « pas de
   trailing » casse `TOPIC`.
2. **Les espaces préservés dans le trailing** (§3.4) : trimmer le trailing
   casse `PRIVMSG` (messages tronqués / altérés).
