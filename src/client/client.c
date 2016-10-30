/*
 * Client utilizing the Gomoku Inter Process Shuttle (GIPS) protocol designed by Sean Batzel and Andrew Plaza.
 *
 * Client designed by Sean Batzel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include "../lib/network.h"
#include "../lib/gips.h"
#include "../lib/glogic.h"

#define HTTPPORT "32200"
#define BACKLOG 10

void send_move(int a, int b, char board[][DEPTH], int sock, short player);
char **get_move(char board[][DEPTH], int sock, short which_player);
void display_board(char *board);

int main() {
    // TODO We need a variable we can pass around to keep track of
    // TODO what player number we current are.
    char *name;
    short which_player;
    int move_x;
    int move_y;
    char board[HEIGHT][DEPTH];
    int sock = connect_to_server();
    printf("Gomoku Client for Linux\n");

    if (sock != -1) {
        printf("Enter your name: ");
        scanf("%s", name);
        send(sock, name, sizeof(name), 0);
        recv(sock, &which_player, sizeof(which_player), 0);
    } else {
        printf("Couldn't connect to the server.\n");
        exit(0);
    }
    while (1) {
        //TODO check this loop
        printf("%s> ", name);
        scanf("%d%d", &move_x, &move_y);
        send_move(move_x, move_y, board, sock, which_player);
        board = get_move(board, sock, which_player);
    }
    close(sock);
}

void send_move(int a, int b, char board[][DEPTH], int sock, short player) {
    board[a][b] = 'x';
    // Send the move to the other guy.
    gips *z = pack(player, 0, !player, a, b);
    send_to(z, sock);
}

char **get_move(char board[][DEPTH], int sock, short which_player) {
    // TODO This needs to take a MOVE and apply it to the board.
    // Get the move from the other guy.
    gips *z; 
    recv(sock, z, sizeof(z), 0);

    // Get an x and y coordinate from the gips packet.
    if (z->isWin != 0) {
        // This needs to be changed to the current player's number.
        if (z->isWin == which_player) {
            printf("You won!");
        } else {
            printf("You lost.");
        }
    }
    // Check if the game is over.
    // Otherwise we just decode
    board[z->move_a][z->move_b] = 'B';
    return board;
}

void display_board(char *board) {
    int i;
    int j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}
