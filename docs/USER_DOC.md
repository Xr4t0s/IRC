# USER DOC

## Compilation

Compiler le projet :

```bash
make
```

Nettoyer les fichiers objets :

```bash
make clean
```

Nettoyer complètement :

```bash
make fclean
```

Recompiler entièrement :

```bash
make re
```

---

## Lancement du serveur

Syntaxe :

```bash
./ircserv <port> <password>
```

Exemple :

```bash
./ircserv 6667 passaaaa
```

Le serveur écoute alors sur le port `6667`.

---

## Connexion avec Netcat

Ouvrir un terminal :

```bash
nc 127.0.0.1 6667
```

Puis envoyer :

```text
PASS passaaaa
NICK kratos
USER kratos 0 * :Kratos
```

Une fois connecté :

```text
JOIN #42
```

Envoyer un message :

```text
PRIVMSG #42 :Bonjour tout le monde
```

Quitter :

```text
QUIT :Bye
```

---

## Connexion avec HexChat

### Installation

Ubuntu :

```bash
sudo apt install hexchat
```

### Création d'un serveur

1. Ouvrir HexChat.
2. Ajouter un nouveau réseau.
3. Modifier le réseau.
4. Ajouter :

```text
127.0.0.1/6667
```

5. Définir un pseudo.
6. Cliquer sur "Connect".

### Password serveur

Si le serveur demande un mot de passe :

```text
/pass passaaaa
```

ou configurer le mot de passe dans les paramètres du réseau.

---

## Commandes IRC utiles

Changer de pseudo :

```text
/nick nouveauPseudo
```

Rejoindre un channel :

```text
/join #42
```

Quitter un channel :

```text
/part #42
```

Envoyer un message privé :

```text
/msg kratos Salut
```

Quitter le serveur :

```text
/quit
```

---

## Débogage

Pour observer exactement ce que le client envoie au serveur, il est conseillé de commencer les tests avec :

```bash
nc 127.0.0.1 6667
```

Cela permet de vérifier facilement les commandes IRC avant d'utiliser un client complet comme HexChat.
