#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gips.h"

void print_board(char **board);

//just for testing GIPS methods int main(){
int main(){
  int i, j;
  char **board;
  short player = 1; 

  board = malloc(HEIGHT * sizeof(char *));
 
  for(i = 0; i < HEIGHT; i++){
    board[i] = malloc(DEPTH);
    memset(&board[i], 0, sizeof(board[i])* strlen(board[i]));
  }

  
  //set test values
  gips *info;

  for(i = 0; i < HEIGHT; i++){
    for(j = 0; j < DEPTH; j++) {
      board[i][j] = 'o';
    }
  }

  board[1][3] = 'x';
  board [5][5] = 'x';

  print_board(board);
  printf("\n");
  info = pack(board, player);
  board = unpack(info);
  print_board(board);


}
void print_board(char **board){
  int i, j;
  for(i = 0; i < HEIGHT; i ++){
    for(j = 0; j < DEPTH; j++){
      printf("%c", board[i][j]);
    }
    printf("\n");
  }
}

gips *pack(char **pos, short player){
  static gips info;
  int i, j;
  long long num = 0;
  
  //I will assume it's size 0-7 for now
  //    (should probs fix this)
  // sets the bit i+j to 1
  for(i = 0; i < 8; i++)  {
    for(j = 0; j < 8;j++) {
      if(pos[i][j] == 'x' )
        num ^= (-1 ^ num) & (1 <<  (i+j));
      //else
        //num ^=(-0 ^ num) & (1 << (i+j));
    }
  }

  info.pos = num;
  info.player = player;
  return &info;
}

//can unpack the same way, except with AND
//x &= (1ull << i + j);
//back to math hw x.x
char **unpack(gips *info){
  int i, j;

  static char *pos[8];
  for(i = 0; i < 8; i++){
    pos[i] = malloc(8);
    memset(&pos[i], 0, sizeof(pos[i])* strlen(pos[i]));
  }

  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++) {
      if( ((info->pos >> (i+j)) & 1) == 1 ){
        pos[i][j] = 'x';
      }else{
        pos[i][j] = 'o';
      }
    }
  }

  return pos;
}

