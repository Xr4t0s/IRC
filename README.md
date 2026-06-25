# IRC Server (C++98)
> *This project has been created as part of the 42 curriculum by nitadros, ...*

IRC (Internet Relay Chat) is an old text-based communication protocol. It allows users to join and leave discussions, send messages, create channels, and interact with other users in real time.

## Project overview

This project consists of building an IRC server from scratch using C++98.

The goal is to reproduce the core behavior of a real IRC server while respecting
strict networking constraints:

- TCP communication
- Non-blocking I/O
- Multiple simultaneous clients
- Event driven architecture
- Single poll() loop

## How IRC works

IRC works around one central thing : the server.

Users connect first to the server, and are asked to identify themselves by sending theses tcp messages :
- `PASS` \<server_password>
- `NICK` \<nickname>
- `USER` \<username>

When everything is valid, the user becomes authenticated.

From there, users can:

- Join channels
- Send private messages
- Send messages inside channels
- Receive messages from others
- Become channel operators

A channel is basically just a discussion room:

    #42
    #gaming
    #linux

Example:

    Alice connect to server
    ↓
    PASS password
    ↓
    NICK alice
    ↓
    USER alice 0 * :Alice
    ↓
    JOIN #42
    ↓
    Bob JOIN #42
    ↓
    Alice:
    PRIVMSG #42 hello
    ↓
    Server receive message
    ↓
    Server find every user inside #42
    ↓
    Server forward message
    ↓
    Bob receive: hello

The server is always the middle point.

Users connect to the server.

Channels live inside the server.

The server decides:

- who is connected
- who joined what
- who can talk
- who is operator
- who can join restricted channels

## Project architecture
```
IRC/
├── Makefile
├── src/
├── include/
├── README.md
├── TASKS.md
├── USER_DOC.md
└── DEV_DOC.md
```
TODO! Explain each parts 

## Requirements

The subject has several requirements that we will explain here:

- The server must be capable of handling multiple clients simultaneously without hanging.

- Forking is prohibited. All I/O operations must be non-blocking.

- Only one `poll()` or equivalent can be used to handle all I/O operations: reading, writing, listening, accepting new clients, and so on.

- Several IRC clients exist. We have to choose one reference client. This client will be used during the evaluation process.

- The reference client must be able to connect to our server without encountering any error.

- Communication between the client and the server must be done via TCP/IP, using IPv4 or IPv6.

- Using the reference client with our server must feel similar to using it with an official IRC server. However, we only have to implement the following features:

    - A client must be able to authenticate, set a nickname, set a username, join a channel, send messages, and receive private messages.

    - All messages sent from one client to a channel must be forwarded to every other client that joined the same channel.

    - The server must support regular users and channel operators.

    - The following channel operator commands must be implemented:

        - `KICK`: eject a client from the channel.

        - `INVITE`: invite a client to a channel.

        - `TOPIC`: change or view the channel topic.

        - `MODE`: change the channel modes:

            - `i`: set/remove invite-only mode.

            - `t`: set/remove the restriction of the `TOPIC` command to channel operators only.

            - `k`: set/remove the channel key/password.

            - `o`: give/take channel operator privilege.

            - `l`: set/remove the user limit of the channel.