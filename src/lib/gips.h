#ifndef GIPS
#define GIPS
typedef struct {
  char pid;
  char isWin;
  char isTurn;
  char move_a;
  char move_b;
} gips;

gips *pack(char pid,char isWin, char isTurn, char move_x, char move_y);
//char **unpack(gips *info);
//void someone_won(gips *x);
#endif /*GIPS*/

#ifndef DEPTH
#define DEPTH 8
#endif
#ifndef HEIGHT
#define HEIGHT 8 
#endif
