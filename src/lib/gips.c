#include "gips.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Some functions to compress what we send over the nets

//packs info needed for playing the game into a struct
//using chars to make the package as small as possible
gips *pack(char pid, char isWin, char move_a, char move_b) {
  static gips info;

  info.pid = pid; //1 if player1, 2 if player2
  info.isWin = isWin; //0 if not win, 1 if player 1 win, 2 if player 2 win
  info.move_a = move_a; //move || first board == own player turn,
  info.move_b = move_b; //move         EVERY OTHER board === OTHER PLAYER move
  return &info;
}

//Some functions to compress what we send over the nets
//gets packed int a long long (64bits/8bytes)
//this was a earlier idea that was switched out for the new gips
// structure. It stays commented since we have some idea for it in the future
// ((i*HEIGHT) + j ) to get the right pos of a 2d array
//packs positions of player into a long long
/*gips *to_long(char **pos, short player){
static gips info;
int i, j;
lng long num = 0;

for(i = 0; i < HEIGHT; i++)  {
for(j = 0; j < DEPTH; j++) {
if(pos[i][j] == 'x' )
num |= (1ull << (((i*HEIGHT)+j)));
}
}

//info.pos = num;
info.pid = player;
return &info;
}


//unpacks by checking if the bit in the ll
//is toggled (1)
//returns fully built board
char **from_long(gips *info){
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
