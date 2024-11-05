#include "client.h"

Client::Client(const std::string& server_ip, int server_port)
    : server_ip(server_ip), server_port(server_port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("Error initializing Winsock");
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        WSACleanup();
        throw runtime_error("Error creating socket");
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(client_socket);
        WSACleanup();
        throw runtime_error("Failed to connect to the server");
    }
}

Client::~Client() {
    closesocket(client_socket);
    WSACleanup();
}

void Client::send_message(const std::string& message) {
    send(client_socket, message.c_str(), static_cast<int>(message.size()), 0);
}

string Client::receive_message() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        throw runtime_error("Connection closed by server");
    }
    return string(buffer, bytes_received);
}

void Client::start() {
    cout << "Connected to the server." << endl;
    cout << "Welcome to the Gaming server XO!" << endl;

    while (true) {
        try {
            string server_message = receive_message();
            cout << server_message << endl;

            if (server_message.find("Your move") != string::npos) {
                string move;
                cout << "Enter your move: ";
                getline(cin, move);
                send_message(move);
            }
            else if (server_message.find("Game over") != string::npos) {
                break;
            }
        }
        catch (const std::exception& e) {
            cerr << e.what() << endl;
            break;
        }
    }
}