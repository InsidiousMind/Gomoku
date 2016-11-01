/*
 * Client utilizing the Gomoku Inter Process Shuttle (GIPS) protocol designed by Sean Batzel and Andrew Plaza.
 *
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
#include <errno.h>
// This file compiles with glogic.h

#define HTTPPORT "32200"
#define BACKLOG 10

void send_move(int a, int b, char **board, int sock, char player);
char **get_move(char **board, int sock, char which_player);
void display_board(char **board);

int main() {
    char *name = malloc(sizeof(char) * 15);
    gips player_info;
    int move_x;
    int move_y;
    char **board = malloc(HEIGHT * sizeof(char*));
    int i;
    for (i = 0; i < HEIGHT; i++) {
        board[i] = malloc(DEPTH * sizeof(char *));
    }
    int sock = connect_to_server();
    printf("Gomoku Client for Linux\n");

    if (sock != -1) {
        printf("Enter your name: ");
        scanf("%s", name);
        send_mesg(name, sock);
        recv(sock, &player_info, sizeof(player_info), 0);
    } else { // Does this go through correctly in the first place?
        printf("Couldn't connect to the server. Error number: ");
        printf("%d\n", errno);
        exit(0);
    }
    while (board != NULL) {
        printf("%s> ", name);
        scanf("%d%d", &move_x, &move_y);
        send_move(move_x, move_y, board, sock, player_info.pid);
        board = get_move(board, sock, player_info.pid);
        display_board(board);
        board = get_move(board, sock, player_info.pid);
        display_board(board);
    }
    close(sock);
    free(board);
    free(name);
}

void send_move(int a, int b, char **board, int sock, char player) {
    board[a][b] = 'x';
    // Send the move to the other guy.
    gips *z = pack(player, 0, !player, a, b, 1);
    send_to(z, sock);
}

char **get_move(char **board, int sock, char which_player) {
    gips *z = malloc(sizeof(gips));
    recv(sock, z, sizeof(z), 0); // FIXME The client hangs here
    //FIXME while waiting for a server response.

    // Get an x and y coordinate from the gips packet.
    if (z->isWin != 0) {
        // This needs to be changed to the current player's number.
        if (z->isWin == which_player) {
            printf("You won!");
        } else {
            printf("You lost.");
        }
        return NULL;
    }
    // Check if the game is over.
    // Otherwise we just decode
    board[(int)z->move_a][(int)z->move_b] = 'B';
    return board;
}

void display_board(char **board) {
    int i;
    int j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}
