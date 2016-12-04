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
#include <stdbool.h>

//shared libraries
#include "../lib/database.h"
#include "../lib/gips.h"
#include "../lib/IO_sighandle.h"

//private functions
#include "commons/client_connect.h"

int gameLoop(gips **player_info, char **name, int sock, char pid);
char getStone(char pid);
char getOtherStone(char pid);
void send_move(int a, int b, char **board, int sock, char player, char stone);
char **init_board(char **board);
void display_board(char **board);
int checkValid(int *moves, char stone, char *name, char **board );
void print_player(Player *play);
void get_move(char ***t_board, gips *z, char stone);
void send_move(int a, int b, char **board, int sock, char player, char stone);
void establish_connection(int sock, int *uniquePID, char **username, int *pid);

int main() {

  char *name = calloc(15, sizeof(char));
 
  char *win = calloc(13, sizeof(char));

  gips *player_info = calloc(sizeof(gips), sizeof(gips*));
  
  int uniquePID, pid = 0, sock;
  
  
  printf("Username: ");
  readWord(name, (int) (strlen(name) + 1));
  printf("Player ID: ");
  scanf("%d", &uniquePID);

  printf("Gomoku Client for Linux\n");

  bool keepPlaying = true;
  int isWin;
  while(keepPlaying) {
    sock = connect_to_server();
    establish_connection(sock, &uniquePID, &name, &pid);
    isWin = gameLoop(&player_info, &name, sock, (char) pid);
    if (player_info->isEarlyExit == -1) {
      printf("\n The other client has Disconnected. Connect to another client waiting to play? "
                 "[Y/n]: ");
      //grab the newline first
      char resp = (char) getchar();
      resp = (char) getchar();
      if (resp == 'Y' || resp == 'y') {
        send(sock, &resp, sizeof(char), 0);
        close(sock);
        keepPlaying = true;
      } else exit(0);
    }else keepPlaying = false;
  }

  if(isWin != pid)
    printf("You Lose! :-(\n");
  else
    printf("You Win!! :-)\n");
  
  Player *player = malloc(sizeof(Player));
 
  recv(sock, player, sizeof(Player), 0);
  print_player(player);

  close(sock);

  free(name);
  free(win);
  free(player_info);
}

void send_move(int a, int b, char **board, int sock, char player, char stone) {
  // Send the move to the other guy.
  gips *z;
  board[a][b] = stone;
  z = pack(player, false, (BYTE) a, (BYTE) b, false);
  send_to(z, sock);
}

void get_move(char ***t_board, gips *z, char stone) {
  char **board = *t_board;
  // Check if the game is over.
  // Otherwise we just decode
  board[(int)z->move_a][(int)z->move_b] = stone;
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

  if(moves[0] < 0 || moves[1] < 0 || moves[0] > (HEIGHT-1) || moves[1] > (HEIGHT-1) ){
    printf("Invalid input.\n");
    return true;
  }
  else if(board[moves[0]][moves[1]] != 'o' && board[moves[0]][moves[1]] != stone){
    printf("You can't take the other players move!\n");
    return true; 
  }else{
    return false;
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


int gameLoop(gips **player_info, char **name, int sock, char pid){
  
  gips *p_info = *player_info;
  
  int i = 0;
  char **board = malloc(HEIGHT * sizeof(char *));
  
  int isWin;
  for (i = 0; i < HEIGHT; i++) {
    board[i] = malloc(DEPTH * sizeof(char *));
  }
  int *moves = calloc(2, sizeof(int));
  
  char stone = getStone(pid);
  char otherStone = getOtherStone(pid);
  
  board = init_board(board);
  
  while (board != NULL) {

    printf("Wait your turn!\n");
    
    recv(sock, p_info, sizeof(p_info), MSG_WAITALL);
    
    //break if win, if first turn don't do anything, else get the move
    if(p_info->isWin != 0) return p_info->isWin;
    else if (p_info->isEarlyExit != 0) return -1;
    else if ((p_info->move_a == -1) && (p_info->move_b == -1)) /*nothing*/;
    else get_move(&board, p_info, otherStone);
   
    display_board(board);

    printf("Now you can move\n");

    signal(SIGINT, INThandle);

    //check for a valid turn 
    while(checkValid(moves, stone, *name, board));
    
    send_move(moves[0], moves[1], board, sock, pid, stone);

    //check for win
    display_board(board);
    recv(sock, &isWin, sizeof(int), 0);
    if (isWin != 0)
      break;
  }
 
  for (i = 0; i < HEIGHT; i++) {
    free(board[i]);
  }

  free(board);
  free(moves);
  
  return isWin;

}

char getStone(char pid){
    if(pid == 1) return 'B';
    else return 'W';
}
char getOtherStone(char pid){
  if(pid == 1) return 'W';
  else return 'B';
}
//sets variables pid etc
void establish_connection(int sock, int *uniquePID, char **username, int *pid){
  
  char *name = *username;
  
  if (sock != -1) {
    *uniquePID = login(sock, *uniquePID, name) ;
    recv(sock, pid, sizeof(char), MSG_WAITALL);
    if(errno || pid == 0 || *pid == -1) {
      printf("shutdown");
      exit(1);
    }
    //TODO
    //Inform user of Unique PID (If it was the one they requested or different)
  } else {
    printf("Couldn't connect to the server. Error number: ");
    printf("%d\n", errno);
    exit(0);
  }
}
