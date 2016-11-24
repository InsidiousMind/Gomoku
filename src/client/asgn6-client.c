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
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>

//shared libraries
#include "../lib/database.h"
#include "../lib/gips.h"
#include "../lib/IO_sighandle.h"

//private functions
#include "commons/client_connect.h"


void send_move(int a, int b, char **board, int sock, char player, char stone) {
  // Send the move to the other guy.
  gips *z;
  board[a][b] = stone;
  z = pack(player, FALSE, a, b);
  send_to(z, sock);
}

char **get_move(char **board, gips *z, char stone) {
  // Check if the game is over.
  // Otherwise we just decode
  board[(int) z->move_a][(int) z->move_b] = stone;
  return board;
}

void print_player(Player *play){

  printf("%s%s%s%d%s\n", "Your Stats for username ", play->username,
                         " and unique ID ", play->userid, " are: \n");
  printf("%s%d\n", "Wins: ", play->wins);
  printf("%s%d\n", "Losses: ", play->losses);
  printf("%s%d\n", "Ties: ", play->ties);
}

int checkValid(int *moves, char stone, char *name, char **board ){

  printf("%s_> ", name);

  int i = 0;
  while(readInts(moves, 2, &i));

  //decrement so that it will fit on board
  moves[0]--;
  moves[1]--;

  if(moves[0] < 0 || moves[1] < 0 || moves[0] > 7 || moves[1] > 7){
    printf("Invalid input.\n");
    return TRUE;
  }
  else if(board[moves[0]][moves[1]] != 'o' && board[moves[0]][moves[1]] != stone){
    printf("You can't take the other players move!\n");
    return TRUE; 
  }else{
    return FALSE;
  }

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
  int i; 

  char *name = calloc(15, sizeof(char));
  char *win = calloc(13, sizeof(char));
  gips *player_info = calloc(sizeof(gips), sizeof(gips*));
  int *moves = calloc(2, sizeof(int));
  
  char pid;
  int uniquePID;
  char stone, otherStone;

  int sock = connect_to_server();

  printf("Username: ");
  readWord(name, strlen(name)+1);
  printf("Player ID: ");
  scanf("%d", &uniquePID);


  //send username
  //send PID

  // Login will reassign a PID if that one isn't right,
  // or will just let them keep the one they supplied.

  char **board = malloc(HEIGHT * sizeof(char *));
  int isWin;

  for (i = 0; i < HEIGHT; i++) {
    board[i] = malloc(DEPTH * sizeof(char *));
  }

  board = init_board(board);
  printf("Gomoku Client for Linux\n");

  //Name and stuff
  if (sock != -1) {
    uniquePID = login(sock, uniquePID, name) ;
    recv(sock, &pid, sizeof(char), 0);
    //TODO
    //Inform user of Unique PID (If it was the one they requested or different)
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
  while (board != NULL) {

    printf("Wait your turn!\n");
    
    signal(SIGINT, INThandle);
    recv(sock, player_info, sizeof(player_info), 0);
    if (player_info->isWin != 0) {
      break;
    } else if ((player_info->move_a == -1) && (player_info->move_b == -1)){
    } else {
      board = get_move(board, player_info, otherStone);
    }
    display_board(board);

    printf("Now you can move\n");

    signal(SIGINT, INThandle);

    //check for a valid turn 
    while(checkValid(moves, stone, name, board));
    
    send_move(moves[0], moves[1], board, sock, pid, stone);

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
  
  Player *player = malloc(sizeof(Player));
 
  recv(sock, player, sizeof(Player), 0);
  print_player(player);

  close(sock);

  for (i = 0; i < HEIGHT; i++) {
    free(board[i]);
  }

  free(board);
  free(name);
  free(win);
  free(moves);
  free(player_info);
}
