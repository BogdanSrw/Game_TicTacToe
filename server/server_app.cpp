#include "server.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage(only for Windows): server_app.exe <port>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    cout << "Gaming server XO version 0.54.2" << endl << endl;
    try {
        Server server(port); // Creating server on mentioned port
        server.start();      // Launching server, expecting connection of 2 players.
    }
    catch (const exception& e) {
        cerr << "Server error: " << e.what() << endl;
        return 1;
    }

    return 0;
}