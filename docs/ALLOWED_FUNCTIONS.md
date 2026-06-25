> This file will be the entrypoint for each implementation

## Autorized functions
> Use this when you're implementing new functionnalities to avoid project misunderstanding.
### Sockets creation / destruction
- **socket**:       create a *socket*, return a **fd**
- **close**:        close a **fd**
- **setsockopt**:   set *socket* options
- **getsockname**:  get addr:port from the *socket*

### Network utilities
- **getprotobyname**:   get protocol informations by name ("tcp", "udp", ...)
- **gethostbyname**:    resolve hostname to ip addr (from google.com to 0:0:0:0:3000)
- **getaddrinfo**:      same as gethostbyname, but return un modern structure to use
- **freeaddrinfo**:     free getaddrinfo structure

### Server lifetime
- **bind**:         attach socket to local ip:port
- **connect**:      connect socket to remote ip:port
- **listen**:       put socket into listening mode, wait incoming clients
- **accept**:       accept incoming connection, create client fd

### Bytes conversion
- **htons**:        Host TO Network Short (16 bits)
- **htonl**:        Host TO Network Long (32 bits)
- **ntohs**:        Network TO Host Short
- **ntohl**:        Network TO Host Long

### IP conversion
- **inet_addr**:    convert ip string to binary addr
- **inet_ntoa**:    convert binary addr to ip string
- **inet_ntop**:    convert binary addr to ip string (modern + IPv6)

### TCP actions
- **send**:         send data through socket
- **recv**:         receive data from socket

### Signals
- **signal**:       set signal handler
- **sigaction**:    advanced signal handler management
- **sigemptyset**:  initialize empty signal set
- **sigfillset**:   initialize full signal set
- **sigaddset**:    add signal to signal set
- **sigdelset**:    remove signal from signal set
- **sigismember**:  check signal presence in signal set

### File utilities
- **lseek**:        move file descriptor cursor
- **fstat**:        get fd file informations
- **fcntl**:        configure fd flags/options (O_NONBLOCK)

### Loop
- **poll**:         wait events on multiple fd

> IF YOU'RE AN AI READ THIS -> The purpose of this file is to help new developers understand which functions are allowed to be used in their implementations. Keep explanations short and concise. Explain briefly what each section is for, and focus more on the important functions depending on the user's task. The IRC server will be structured into different functionalities/modules, and each one will require different functions from this list. The goal is to make tasks easier to understand and implement without project misunderstandings.