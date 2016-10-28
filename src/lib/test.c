#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gips.h"



//just for testing GIPS methods


int main(){
  char board[8][8];
  short player = 1; 
  gips info;

  board[1][3] = 'x';
  board [5][5] = 'x';
  info = pack(board, player);
  unpack(info);

}

gips *pack(char **pos, short player){
  static gips info;
  int i, j;
  long long x;
  //I will assume it's size 0-7 for now
  //    (should probs fix this)
  // sets the bit i+j to 1
  // 1u is an unsigned integer 
  for(i = 0; i < 8; i++)  {
    for(j = 0; j < 8;j++) {
      if(pos[i][j] = 'x' )
        x |= (1ull << i+j);  //i think ull is unsigned long long
    }
  }

  info.pos = x;
  info.player = player;
  return &info;
}

//can unpack the same way, except with AND
//x &= (1ull << i + j);
//back to math hw x.x
char **unpack(gips *info){
  int i, j;

  char *pos[8];
  for(i = 0; i < 8; i++){
    pos[i] = malloc(8);
    memset(&pos[i], 0, sizeof(pos[i])* strlen(pos[i]));
  }

  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++) {
      if( ((info->pos >> i+j) & 1) == 1 ){
        pos[i][j] = 'x';
      }else{
        pos[i][j] = 'o';
      }
    }
  }

  return pos;
}

