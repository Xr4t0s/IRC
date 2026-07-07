#include "core/Server.hpp"

extern volatile sig_atomic_t g_running;

void    sig_handler(int code) {
    static_cast<void>(code);
    g_running = 0;
}

int main(int ac, char **av) {
    
    if (ac != 3) {
        std::cout << "Usage: ./ircserver <password> <port>\n";
        return 1;
    }    

    Server serv;
    
    signal(SIGINT, sig_handler);

    if (!serv.init(++av))
        return 1;

    try {
        serv.run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}