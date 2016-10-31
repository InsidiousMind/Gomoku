#ifndef GIPS
#define GIPS
typedef struct {
  char pid;
  char isWin;
  char whoTurn;
  char waiting;
  char move_a;
  char move_b;
} gips;

typedef struct {
  char pid;
  long long comp_board;
} gips_comp;

gips *to_long(char **pos, short player); //packs long long
char **from_long(gips *info);
gips *pack(char pid,char isWin, char isTurn, char move_x, char move_y, char waiting);
//void someone_won(gips *x);

#endif /*GIPS*/

#ifndef DEPTH
#define DEPTH 8
#endif
#ifndef HEIGHT
#define HEIGHT 8
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
