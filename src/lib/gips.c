#include "gips.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
//if the game logic is server-side, this is unnecessary
// It's actually not unnecessary - the game logic determines if we won,
// but the client needs to be responsible for telling the player and
// quitting itself. - Sean
*/
/*void someone_won(gips *x) {
	// Ask the server if someone won?
	// Takes a single 0 or 1 to tell us if we won or not.
	// 1 is a loss.
	switch (x->move) {
		case 1:
			printf("You've lost.");
		case 0:
			printf("You win!");
	}
	exit(0);
} */

//packs positions of player into a long long
//stored on a per-player basis, cannot store
//both players in one long long because
//long long is only 64bits 8*8 64 so perfecto ;D

gips *pack(char **pos, short player){
  static gips info;
  int i, j;
  long long num = 0;
  
  //I will assume it's size 0-7 for now
  //    (should probs fix this)
  // sets the bit i+j to 1
  for(i = 0; i < HEIGHT; i++)  {
    for(j = 0; j < DEPTH; j++) {
      if(pos[i][j] == 'x' )
        num |= (1ull << (((i*HEIGHT)+j)));
    }
  }

  info.pos = num;
  info.player = player;
  return &info;
}

//can unpack the same way, except with AND
//x &= (1ull << ((i*HEIGHT) + j));
char **unpack(gips *info){
  int i, j;

  static char *pos[8];
  for(i = 0; i < 8; i++){
    pos[i] = malloc(8);
    memset(&pos[i], 0, sizeof(pos[i])* strlen(pos[i]));
  }

  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++) {
      if( ((info->pos >> ((i*HEIGHT)+j)) & 1) == 1 ){
        pos[i][j] = 'x';
      }else{
        pos[i][j] = 'o';
      }
    }
  }

  return pos;
}
