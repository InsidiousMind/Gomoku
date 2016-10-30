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



//Some functions to compress what we send over the nets

//packs info needed for playing the game into a struct
//using chars to make the package as small as possible
gips *pack(char pid,char isWin, char isTurn, char move_x, char move_y){
  static gips info;
  
  info.pid = pid; //1 if player1, 2 if player2
  info.isWin = isWin; //0 if not win, 1 if player 1 win, 2 if player 2 win
  info.isTurn = isTurn; //0 if not turn, 1 if turn
  info.move_x = move_x;
  info.move_y = move_y;  
  
  return &info;
}


/*
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

*/
