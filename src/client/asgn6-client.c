/*Author: Sean Batzel and Andrew Plaza
 * Date: October 31 2016
 * Github: https://github.com/InsidiousMind/Gomoku
 * File name: asgn6-client.c
 * compile: make server
 * run ./server
 * debug: gdb ./server
 *
 * A client program that communicates with a server in order to play
 * Gomoku (five in a row)
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
#include <errno.h>

#define HTTPPORT "32200"
#define BACKLOG 10

int login() {
  // The server needs logic to check if the database already contains a user,
  // and to assign the client a pid. pid_from_server should return a pid that
  // the client was sent from the server.
  char *username = malloc(sizeof(char) * 20);
  printf("Player username: ");
  scanf("%s", username);
  int pid = pid_from_server(username); // We should implement this in network.h
}

void send_move(int a, int b, char **board, int sock, char player) {
  // Send the move to the other guy.
  gips *z = malloc(sizeof(gips));
  board[a][b] = 'W';
  z = pack(player, FALSE, a, b);
  send_to(z, sock);
}

char **get_move(char **board, gips *z) {
  // Check if the game is over.
  // Otherwise we just decode
  board[(int) z->move_a][(int) z->move_b] = 'B';
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

char **init_board(char **board) {
  int i, j;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < DEPTH; j++) {
      board[i][j] = 'o';

    }

  }
  return board;
}



//make sure scanf only scans upto 15 characters, and assigns nullbyte at the end
int main() {
  char *name = malloc(sizeof(char) * 15);
  char *win = malloc(sizeof(char) * 13);
  gips *player_info = malloc(8 * sizeof(char));
  int move_x, move_y, i;
  char pid;

  char **board = malloc(HEIGHT * sizeof(char *));
  int isWin;
  for (i = 0; i < HEIGHT; i++) {
    board[i] = malloc(DEPTH * sizeof(char *));
  }
  board = init_board(board);
  int sock = connect_to_server();
  printf("Gomoku Client for Linux\n");

  if (sock != -1) {
    printf("Enter your name: ");
    scanf("%s", name);
    send_mesg(name, sock);
    recv(sock, player_info, sizeof(player_info), 0);
    board = get_move(board, player_info);
    pid = player_info->pid;
    display_board(board);
  } else {
    printf("Couldn't connect to the server. Error number: ");
    printf("%d\n", errno);
    exit(0);
  }

  while (board != NULL) {
    printf("Wait your turn!\n");
    recv(sock, player_info, sizeof(player_info), 0);
    if (player_info->isWin != 0) {
      break;
    }
    board = get_move(board, player_info);
    display_board(board);
    printf("Now you can move\n");
    printf("%s_> ", name);
    scanf("%d%d", &move_x, &move_y);
    send_move(move_x, move_y, board, sock, pid);
    //check for win
    recv(sock, &isWin, sizeof(int), 0);
    if (isWin != 0)
      break;
  }
  recv(sock, win, sizeof(char) * 14, 0);
  printf("%s\n", win);
  close(sock);

  for (i = 0; i < HEIGHT; i++) {
    free(board[i]);
  }
  free(board);
  free(name);
  free(win);
  free(player_info);
}
