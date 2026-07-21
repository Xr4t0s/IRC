#include "core/Server.hpp"

volatile sig_atomic_t g_running = 1;

Server::Server() : _fd(-1), _port(-1), _efd(-1), _cmdHandler(*this) {}

bool Server::init(char **args) {
    std::cout << "Server stack created\n";
    
    try {
        _init_pass(args[0]);
        _init_port(args[1]);
        _init_socket();
        _init_listening();
        _init_epoll();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

void Server::_init_pass(char *pass) {
    if (!pass)
        throw Error("Missing password");

    _pass = std::string(pass);
    
    // Mini parsing du password
    if (_pass.length() < 8)
        throw Error("Illegal password");
}

void Server::_init_port(char *port) {
    // Pareil pour le port
    for (int i = 0; port[i]; i++) {
        if (!isdigit(port[i]))
            throw Error("Illegal port");
    }

    _port = atoi(port);
    // Verifie si pas ports root ou invalide
    if (_port < 1024 || _port > 65535)
        throw Error("Illegal port");
}

/// Creation d'une socket aka un pipe reseau, l'autre côté est géré par le kernel
void Server::_init_socket() {
    /*
        La socket côté kernel est une structure configuré de plusieurs manières différentes, protocol, address, type etc...
        Ici socket crée une structure kernel, puis renvoie un fd qui communique avec celle-ci
            - AF_INET -> IPv4 pour le type d'addresse
            - SOCK_STREAM -> Stream continu pour le type de socket
            - 0 -> TCP pour le protocol (TCP par déduction IPv4 + SOCK_STREAM => TCP)
        Pour le 3ème paramètre de la fonction socket on pourrait être explicite en remplacant 0 par IPPROTO_TCP de <netinet/in.h> mais 0 fonctionne automatiquement
        C'est la toute première étape réseau, on a construit une socket réseau dans le kernel et récupérer l'entrée de celle-ci
    */
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1)
        throw Error("Socket error");
    
    /*
        on a dit que la structure avait des paramètres
        ici on règle le paramètre SO_REUSEADDR pour pouvoir réutiliser l'address rapidement après un Ctrl+C, sinon le kernel la mets 60s en TIME_WAIT
        setsockopt pour set_socket_options prends :
        -   le fd de la socket -> _fd
        -   le groupe de paramètres concerné -> SOL_SOCKET (paramètres sockets globaux)
        -   un pointeur vers un int contenant le flag
        -   la longueur de la variable utilisé pour la valeur
    */
    int option_value = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) == -1) {
        close(_fd);
        throw Error("Socket error");
    }

    /*
        Ici on rend le fd non-bloquant.
        Ça veut dire que accept() recv() et send() ne vont pas arreter le programme.
        Si rien n’est disponible, ils retournent une erreur temporaire comme EAGAIN/EWOULDBLOCK.
            - fd = fd de notre socket
            - F_SETFL -> commande set_flag de fcntl
            - O_NONBLOCK -> flag pour rendre le fd non bloquant 
    */
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
        close(_fd);
        throw Error("Socket error");
    }
}

void Server::_init_listening() {
    /*
        Ici aussi on crée une structure kernel "sockaddr" qui détient toutes les infos de l'addresse locale "localhost, 127.0.0.1, ..."
        afin de lier notre fd a cette structure, et donc utilisé cette "structure addresse"
        Mais on va plutôt créer sockaddr_in qui a une structure plus précise qu'on va pouvoir downcast en sockaddr
            - AF_INET indique encore une fois une addresse de type IPv4
            - htons(_port) -> little endian to big endian pour les bits du port
            - INADDR_ANY pour que ce soit n'importe quelle addresse (0.0.0.0)
            - on sécurise sin_zero jsp cest quoi donc bits à zero
    */
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    for (int i = 0; i < 8; i++) {
        addr.sin_zero[i] = 0;
    }

    /*
        Ici on essaie de bind (lier) la structure addresse construite précédemment avec le fd de notre socket,
        la socket étant crée mais reliée a aucune addresse réseau, ici on fait la connection
        on précise au kernel, plus précisement au fd de la socket crée dans _init_socket, que celle-ci ecoute sur telle addresse avec tel port etc...
        on précise qu'on n'a pas encore activer l'écoute sur cette addresse
        si un paquet TCP arrive sur laddresse, le kernel sait que celle-ci est associé au fd de la socket
        mais pour l'instant c'est impossible dans qu'on appelle pas listen()
    */
    socklen_t sock_len = sizeof(addr);
    if (bind(_fd, reinterpret_cast<sockaddr*>(&addr), sock_len) == -1) {
        close(_fd);
        throw Error("Listening error");
    }

    /*
        Ici on active l'écoute, elle permet alors de recevoir des paquets TCP sur l'addresse bindée
        SOMAXCONN définit le nombre maximum de connexions en attente
    */
    if (listen(_fd, SOMAXCONN) == -1) {
        close(_fd);
        throw Error("Listening error");
    }
}

void Server::_init_epoll() {
    /*
        epoll_create crée une structure epoll dans le kernel :
            elle permet de surveiller plusieurs fd en meme temps sans bloquer
            le kernel renvoie seulement les fd qui sont prêts à etre lu, sinon on passe
            ca permet de gérer plein de fd en meme temps sans devoir bouclé sur une liste qui contient 99% de fd déjà traités

        cette structure surveille des events précis, les epoll_event, donc on en créera un juste après et lajouterons a la watchlist
        avec epoll_ctl()
        on attendra ensuite les fd dans une boucle avec epoll_wait() qui renverra un nombre de fd prets
    */
    _efd = epoll_create1(0);

   if (_efd == -1) {
       close(_fd);
        throw Error("Epoll error");
    }
 
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = _fd;

    /*
        Ici on ajoute l'event a la watchlist, en disant à epoll_wait de prevenir si un event EPOLLIN arrive,
            - EPOLLIN -> préviens quand données arrivent
        le kernel le surveillera et réveillera epoll_wait si besoin
    */
    if (epoll_ctl(_efd, EPOLL_CTL_ADD, _fd, &event) == -1) {
        close(_fd);
        throw Error("Epoll error");
    }
}

void Server::run() {

    // Ici on crée une liste d'events qu'epoll remplira automatiquement pour chaque évènement de taille MAX_EVENTS
    epoll_event events[MAX_EVENTS];

    while(g_running) {
        /*
            Ici epoll_wait permet d'attendre des events des éléments ajouté à la watchlist avec epoll_ctl
                - _efd -> epoll fd car cest une fd
                - events -> structure d'éléments à watch
                - nombre d'event max
                - -1 pour infini
        */
        int counts = epoll_wait(_efd, events, MAX_EVENTS, -1);
        if (counts == -1 && errno != EINTR)
            throw Error("Epoll error");

        for (int i = 0; i < counts; i++) {
            int fd = events[i].data.fd;

            
            // Si le fd concerné est notre serveur alors nous avons un nouveau client à gérer
            if (fd == _fd) {
                std::cout << "New connection\n";
                try {
                    _accept_client();
                } catch (std::exception& e) {
                    static_cast<void>(e);
                }
            }

            /*
                Sinon c'est un des fd clients qui envoie un event,
                c'est ici qu'on traite msg et commande comme KICK etc on doit parse plus bas
            */
            else {
                epoll_event ev = events[i];
                if (ev.events & EPOLLIN) {
                    // on crée un buffer pour recv
                    // todo! mettre un #define CLIENT_BUFFER <uint>
                    char buff[512 + 1];

                    // On lit depuis le fd
                    int nread = recv(fd, buff, 512, 0);
                    if (nread == 0) {
                        // Si aucun bytes lu mais message correct -> fin de connexion
                        std::cout << "Removed client" << std::endl;
                        _remove_client(fd);
                        continue;
                    } else if (nread > 0) {
                        // Ici on imprime juste mais on doit gérer buffer + command ici
                        // todo! implémenter le parsing
                        buff[nread] = '\0';
    
                        Client* client = this->getClientByFd(fd);
                        if (!client)
                            throw Error("Server Misunderstood client");
                            
                        client->fillInBuffer(buff);
                        while (client->hasCompleteCommand())
                            _cmdHandler.execute(*client, parseCommand(client->extractCommand()));
                        
                        std::cout << buff << std::endl;
                        
                        continue;
                    } else if (nread < 0) {
                        _remove_client(fd);
                        continue;
                    }
                } else if (ev.events & EPOLLOUT) {
                    std::cout << "EPOLLOUT" << std::endl;
                    Client* client = this->getClientByFd(fd);
                    if (!client)
                        throw Error("Server Misunderstood client");

                    std::string& clientBuff = client->getOutBuff();

                    ssize_t tmp = send(fd, clientBuff.c_str(), clientBuff.size(), 0);
                    if (tmp == -1)
                        throw Error("Send Unexpected Error");
                    clientBuff.erase(0, tmp);
                    
                    if (clientBuff.empty()) {
                        epoll_event event;
                        event.events = EPOLLIN;
                        event.data.fd = fd;

                        epoll_ctl(_efd, EPOLL_CTL_MOD, fd, &event);
                    }
                }
            }
        }
    }
}

const std::string& Server::getPassword() const
{
    return _pass;
}

int Server::getEfd() const {
    return _efd;
}

Client* Server::getClientByFd(int fd) {
    std::map<int, Client>::iterator it = _clients.find(fd);

    if (it != _clients.end()) {
        return &(*it).second;
    }

    return NULL;
}

Client*     Server::getClientByNick(std::string nick) {
    std::map<int, Client>::iterator it = _clients.begin();

    std::string tmp;

    while (it != _clients.end())
    {
        tmp = (*it).second.getNick();
        if (tmp == nick)
            return &(*it).second;
        it++;
    }

    return NULL;
}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel *>::iterator it = _channels.find(name);

    if (it != _channels.end()) {
        return it->second;
    }

    return NULL;
}

void    Server::_accept_client() {
    // On crée laddress du client comme pour notre serveur
    sockaddr_in addr;
    socklen_t sock_len = sizeof(addr);
    // Puis on accepte le client avec accept(), ce qui crée un nouvel fd pour communiquer avec le client
    int client_fd = accept(_fd, reinterpret_cast<sockaddr*>(&addr), &sock_len);
    if (client_fd == -1) {
        std::cout << "Failed to accept client\n";
        throw ;
    }

    // On passe le fd du nouveau client en mode non bloquant
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "Failed to set client as non-blocking\n";
        close(client_fd);
        throw ;
    }

    // On crée la structure d'event à watch pour ce client
    epoll_event cli_event;
    cli_event.events = EPOLLIN;
    cli_event.data.fd = client_fd;

    // On ajoute la structure dans notre watchlist
    if (epoll_ctl(_efd, EPOLL_CTL_ADD, client_fd, &cli_event) == -1) {
        std::cout << "Failed to add client to watchlist\n";
        close(client_fd);
        throw ;
    }
    
    Client client(client_fd);
    _clients.insert(std::make_pair(client_fd, client));
}

void    Server::_remove_client(int fd) {
    // On remove de la watchlist
    epoll_ctl(_efd, EPOLL_CTL_DEL, fd, NULL);
    // On close le fd concerné et on continue notre boucle normalement
    close(fd);
    _clients.erase(fd);
}

Server::~Server() {
    if (_fd != -1) {
        close(_fd);
    };
    if (_efd != -1) {
        close(_efd);
    }
    std::cout << "Server stack destroyed\n";
}
