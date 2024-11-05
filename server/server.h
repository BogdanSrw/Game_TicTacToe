#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>


#define BOARD_SIZE 3
#define BUFFER_SIZE 1024

using namespace std;

class Server {
public:
    Server(int port);
    ~Server();
    void start();

private:
    SOCKET server_socket;
    SOCKET client_sockets[2];
    int current_player;
    char board[BOARD_SIZE][BOARD_SIZE];
    bool game_over;
    int move_count;

    void initialize_board();
    void display_board();
    bool make_move(int client_id, int position);
    bool check_win(char symbol);
    bool is_draw();
    void send_board();
    void send_message(SOCKET client_socket, const std::string& message);
};