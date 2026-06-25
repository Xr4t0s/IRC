#include "core/Server.hpp"

int main(int ac, char **av) {
    
    if (ac != 3) {
        std::cout << "Usage: ./ircserver <password> <port>\n";
        return 1;
    }    

    Server serv;

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