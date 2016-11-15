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
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#include "../lib/network.h"
#include "../lib/gips.h"
#include "../lib/misc.h"

#define HTTPPORT "32200"
#define BACKLOG 10



void send_move(int a, int b, char **board, int sock, char player, char stone) {
  // Send the move to the other guy.
  gips *z = malloc(sizeof(gips));
  board[a][b] = stone;
  z = pack(player, FALSE, a, b);
  send_to(z, sock);
}

char **get_move(char **board, gips *z, char pid, char stone) {
  // Check if the game is over.
  // Otherwise we just decode
  board[(int) z->move_a][(int) z->move_b] = stone;
  return board;
}

void display_board(char **board) {
  int i;
  int j;
  printf("#");
  for (i = 1; i <= 8; i++) {
    printf(" %d ", i);
  }
  printf("#");
  printf("\n");
  for (i = 0; i < 8; i++) {
    printf("%d", i+1);
    for (j = 0; j < 8; j++) {
      printf(" %c ", board[i][j]);
    }
    printf("%d", i+1);
    printf("\n");
  }
  printf("#");
  for (i = 1; i <= 8; i++) {
    printf(" %d ", i);
  }
  printf("#");
  printf("\n");
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
  gips *player_info = calloc(sizeof(gips), sizeof(gips*));
  int move_x, move_y, i;
  char pid;
  int uniquePID;
  char stone, otherStone;

  int sock = connect_to_server();

  /*printf("Username: ");
  scanf("%s", name);
  printf("Player ID: ");
  scanf("%d", &uniquePID);
  */

  //send username
  //send PID
  
  // Login will reassign a PID if that one isn't right,
  // or will just let them keep the one they supplied.
  //pid = login(sock, &uniquePid, name);

  char **board = malloc(HEIGHT * sizeof(char *));
  int isWin;

  for (i = 0; i < HEIGHT; i++) {
    board[i] = malloc(DEPTH * sizeof(char *));
  }

  board = init_board(board);
  printf("Gomoku Client for Linux\n");
  
  //Name and stuff 
  if (sock != -1) {
    printf("Enter your name: ");
    readWord(name, 15, FALSE);
    send_mesg(name, sock);
    recv(sock, &pid, sizeof(char), 0);
 
    if(pid == 1) {
      stone = 'B';
      otherStone = 'W';
    }
    else{
      stone = 'W';
      otherStone = 'B';
    }
  } else {
    printf("Couldn't connect to the server. Error number: ");
    printf("%d\n", errno);
    exit(0);
  }

  signal(SIGINT, INThandle);

  while (board != NULL) {
    printf("Wait your turn!\n");
    recv(sock, player_info, sizeof(player_info), 0);
    if (player_info->isWin != 0) {
      break;
    } else if ((player_info->move_a == -1) && (player_info->move_b == -1)){
    } else {
      board = get_move(board, player_info, pid, otherStone);
    }
    display_board(board);
    
    printf("Now you can move\n");
    int valid = FALSE;

    while(valid == FALSE) {
      printf("\n%s_> ", name);
      scanf("%d%d", &move_x, &move_y);
      if(move_x < 1 || move_y < 1 || move_x > 8 || move_y > 8)
        printf("Invalid input.");
      else 
        valid = TRUE;
    }

    send_move(--move_x, --move_y, board, sock, pid, stone);
   
    //check for win
    display_board(board);
    recv(sock, &isWin, sizeof(int), 0);
    if (isWin != 0)
      break;
  }

  if(isWin != pid)
    printf("You Lose! :-(\n");
  else
    printf("You Win!! :-)\n");

  close(sock);

  for (i = 0; i < HEIGHT; i++) {
    free(board[i]);
  }

  free(board);
  free(name);
  free(win);
  free(player_info);
}
