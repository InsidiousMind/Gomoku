#include "gips.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
// It's actually not unnecessary - the game logic determines if we won,
// but the client needs to be responsible for telling the player and
// quitting itself. - Sean
*/


//OK fair enough. Uncommented b/c compilation error and i was testing pack and unpack


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



//Some methods to compress what we send over the nets
//gets packed int a long long (64bits/8bytes)

// ((i*HEIGHT) + j ) to get the right pos of a 2d array
//packs positions of player into a long long
gips *pack(char **pos, short player){
  static gips info;
  int i, j;
  long long num = 0;
  
  for(i = 0; i < HEIGHT; i++)  {
    for(j = 0; j < DEPTH; j++) {
      if(pos[i][j] == 'x' )
        num |= (1ull << (((i*HEIGHT)+j)));
    }
  }

  info.pos = num;
  info.pid = player;
  return &info;
}


//unpacks by checking if the bit in the ll
//is toggled (1)
//returns fully built board 
char **unpack(gips *info){
  int i, j;

  static char *pos[8];

  for(i = 0; i < 8; i++){
    pos[i] = malloc(8);
    memset(&pos[i], 0, sizeof(pos[i])* strlen(pos[i]));
  }

  for(i = 0; i < HEIGHT; i++){
    for(j = 0; j < DEPTH; j++) {
      if( ((info->pos >> ((i*HEIGHT)+j)) & 1) == 1 ){
        pos[i][j] = 'x';
      }else{
        pos[i][j] = 'o';
      }
    }
  }

  return pos;
}
