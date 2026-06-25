# INTRODUCTION

> RFC IRC: https://www.rfc-editor.org/rfc/rfc1459.html

## Structure

Le Serveur détiendra les Client et les Channel.

Quand un client se connecte, on check le password, si le nom n'est pas pris etc..., puis on crée la structure Client correspondante.

Les Channel détiendront des pointeurs sur les clients.

Pas des copies.
```cpp
class Client {
    private:
        int         _fd;
        std::string _username;
        std::string _nickname;
        std::string _buff;
};

class Channel {
    public:
        std::vector<Client*> clients;
        std::vector<Client*> operators;
        // ou clients[0]->isOperator();
}

class Serveur {
    public:
        // ...
        std::map<int, Client> clients;
        std::map<std::string, Channel> channels;
}
```

On utilisera donc ca comme suit :
```cpp
int main() {
    // epoll, poll, kqueue...
    while(/* Paquet TCP reçu */) {
        if (/* new client */) {
            try {
                int fd = getFd();
                Client newClient = accept_client();
                clients[fd] = newClient;
                send_success(newClient);
            } catch (std::exception e) {
                send_error(e.what())
            }
        }
        else (/* already connected client */) {}
    }
}
```

## Process
```cpp
int main() {
    // poll, epoll, kqueue...

    while (server_is_running) {
        // attendre des événements sur les fd

        if (/* new client connection */) {
            int fd = accept(...);

            clients[fd] = Client(fd);

            // Le client existe maintenant,
            // mais il n'est pas encore enregistré.
        }
        else if (/* already connected client sent data */) {
            int fd = /* fd prêt à lire */;

            char tmp[1024];
            ssize_t n = recv(fd, tmp, sizeof(tmp), 0);

            if (n <= 0) {
                // client déconnecté ou erreur
                removeClient(fd);
                continue;
            }

            Client& client = clients[fd];

            client.appendBuffer(tmp, n);

            while (client.hasCompleteCommand()) {
                std::string line = client.extractCommand();

                // Exemple :
                // PASS passaaaa
                // NICK kratos
                // USER kratos 0 * :realname

                handleCommand(client, line);

                if (!client.isRegistered()
                    && client.hasValidPass()
                    && client.hasNick()
                    && client.hasUser()) {
                    client.setRegistered(true);
                    sendWelcome(client);
                }
            }
        }
    }
}
```
> **Important**: Il ne faut pas bloquer le serveur en attendant qu’un client envoie `\r\n`.

Chaque client possède son propre buffer : `std::string _buffer;`

À chaque recv(), on ajoute les données reçues dans ce buffer.

Ensuite, on extrait toutes les commandes complètes terminées par : `\r\n`

**Exemple :**
```cpp
while ((pos = client.buffer.find("\r\n")) != std::string::npos) {
    std::string command = client.buffer.substr(0, pos);
    client.buffer.erase(0, pos + 2);

    handleCommand(client, command);
}
```

Si une commande est incomplète, on la garde dans le buffer et on retourne à la boucle principale pour continuer à traiter les autres clients.

>Le gros changement important : **tu ne dois pas attendre PASS/NICK/USER avant de créer le `Client`**. Tu le crées dès `accept()`, puis tu le remplis progressivement.