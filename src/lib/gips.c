#include "gips.h"


/*
//if the game logic is server-side, this is unnecessary
void someone_won(gips *x) {
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
}
*/

//packs positions of player into a long long
//stored on a per-player basis, cannot store
//both players in one long long because
//long long is only 64bits 8*8 64 so perfecto ;D
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
int *unpack(gips *x){
  
}

