#include "client.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: client_app <server_ip> <port>" << std::endl;
        return 1;
    }

    string server_ip = argv[1];
    int port = atoi(argv[2]);

    try {
        Client client(server_ip, port); // Creating a client, connected to the mentioned IP and port
        client.start();                 // Launching main gaming cycle
    }
    catch (const exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}