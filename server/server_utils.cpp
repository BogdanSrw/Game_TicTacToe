#include "server.h"


Server::Server(int port) : game_over(false), move_count(0), current_player(0) {
    initialize_board();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("Error initializing Winsock");
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        WSACleanup();
        throw runtime_error("Error creating socket");
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(server_socket);
        WSACleanup();
        throw runtime_error("Error binding socket");
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(server_socket);
        WSACleanup();
        throw runtime_error("Error listening on socket");
    }

    cout << "Server started. Waiting for players...\n";
}

Server::~Server() {
    closesocket(server_socket);
    WSACleanup();
}

void Server::send_message(SOCKET client_socket, const string& message) {
    send(client_socket, message.c_str(), static_cast<int>(message.size()), 0);
}

void Server::initialize_board() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }
}

void Server::display_board() {
    cout << "Current board:\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << board[i][j] << (j < BOARD_SIZE - 1 ? " | " : "");
        }
        cout << "\n" << (i < BOARD_SIZE - 1 ? "---------" : "") << "\n";
    }
}

void Server::send_board() {
    string board_state = "Board:\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board_state += board[i][j];
            if (j < BOARD_SIZE - 1) board_state += " | ";
        }
        board_state += "\n";
        if (i < BOARD_SIZE - 1) board_state += "---------\n";
    }

    for (int i = 0; i < 2; ++i) {
        send_message(client_sockets[i], board_state);
    }
}

bool Server::make_move(int client_id, int position) {
    int row = (position - 1) / BOARD_SIZE;
    int col = (position - 1) % BOARD_SIZE;
    char symbol = client_id == 0 ? 'X' : 'O';

    if (position < 1 || position > 9 || board[row][col] != ' ') {
        return false;
    }

    board[row][col] = symbol;
    move_count++;
    return true;
}

bool Server::check_win(char symbol) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol)
            return true;
        if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol)
            return true;
    }
    return (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
        (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol);
}

bool Server::is_draw() {
    return move_count == BOARD_SIZE * BOARD_SIZE;
}

void Server::start() {
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    for (int i = 0; i < 2; ++i) {
        client_sockets[i] = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
        if (client_sockets[i] == INVALID_SOCKET) {
            cerr << "Failed to accept client connection\n";
            return;
        }
        send_message(client_sockets[i], "Waiting for the second player...\n");
        cout << "Player " << i + 1 << " connected" << endl;
    }

    send_message(client_sockets[0], "Both players connected! You go first.");
    send_message(client_sockets[1], "Both players connected! Waiting for the other player's move.");
    cout << "Starting gaming session." << endl <<endl;
    send_board();

    while (!game_over) {
        SOCKET current_socket = client_sockets[current_player];
        send_message(current_socket, "Your move. Enter position (1-9): ");

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        recv(current_socket, buffer, BUFFER_SIZE, 0);
        int position = atoi(buffer);
        cout << "Session 1 " << "Player's " << current_player + 1 << " move: " << position << endl;

        if (make_move(current_player, position)) {
            send_board();

            if (check_win(current_player == 0 ? 'X' : 'O')) {
                send_message(client_sockets[0], "Game over! You win!");
                send_message(client_sockets[1], "Game over! You lose!");
                game_over = true;
            }
            else if (is_draw()) {
                send_message(client_sockets[0], "Game over! It's a draw!");
                send_message(client_sockets[1], "Game over! It's a draw!");
                game_over = true;
            }
            else {
                current_player = 1 - current_player;
            }
        }
        else {
            send_message(current_socket, "Invalid move, try again.");
        }
    }
    cout << endl << "Game over. Shutting down the server." << endl;
    for (int i = 0; i < 2; ++i) {
        closesocket(client_sockets[i]);
    }
}