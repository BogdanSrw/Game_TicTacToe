#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

using namespace std;

class Client {
public:
    Client(const string& server_ip, int server_port);
    ~Client();
    void start();

private:
    SOCKET client_socket;
    string server_ip;
    int server_port;

    void send_message(const string& message);
    string receive_message();
};