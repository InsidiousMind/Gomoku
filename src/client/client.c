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

char **init_board(char **board){
  int i,j;
  for(i = 0; i < HEIGHT; i++){
    for(j=0;j<DEPTH;j++) {
      board[i][j] = 'o'; 
      
    }
    
  }
  return board;
}

int main() {
  char *name = malloc(sizeof(char) * 15);
  gips player_info;
  int move_x;
  int move_y;
  char pid;
  char **board = malloc(HEIGHT * sizeof(char*));
  int read_count;
  int i;
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
    read_count = recv(sock, &player_info, sizeof(player_info), 0);
    board = get_move(board, &player_info);
    pid = player_info.pid;
  } else { // Does this go through correctly in the first place?
    printf("Couldn't connect to the server. Error number: ");
    printf("%d\n", errno);
    exit(0);
  }
  char dumbBuff[2];
  while(board != NULL) {
    *dumbBuff = 0;
    printf("Wait your turn!\n");
    //read_count = read(sock, dumbBuff, 3);
    readBytes(sock, 3, dumbBuff );
    read_count = recv(sock, dumbBuff, 3 , MSG_PEEK);
    printf("Now you can move\n");
    display_board(board);
    printf("%s_> ", name);
    scanf("%d%d", &move_x, &move_y);
    send_move(move_x, move_y, board, sock, pid);
    recv(sock, &player_info, sizeof(player_info), 0);
    board = get_move(board, &player_info);
  }
  close(sock);
  free(board);
  free(name);
}

