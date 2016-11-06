#include "glogic.h"
#include "gips.h"

int start_pos_x[HEIGHT];
int start_pos_y[DEPTH];

void find_starts(char **board);

int IsWithinBoard(int x, int y);

int crawl_board(char **board, int startx, int starty);

int check_for_win_server(char **board) {
  char  
  int isWin = 0;
  int i;
  find_starts(board);

  for (i = 0; i < (signed) (sizeof(start_pos_x) / sizeof(int)); i++) {
    isWin = crawl_board(board, start_pos_x[i], start_pos_y[i]);
  }
  return isWin;
}

//two fors w/ two whiles might be possible to avoid
//
/*
* Maybe something like this? instead of iterating through the entire board,
* we can just check if the latest move results in five-in-a-row.
* this sets up two arrays with the directoins we need,
* and loops in that direction until it hits five-in-a-row or the edge of the board
*/
int crawl_board(char **board, int startx, int starty) {
  int i;
  int j;                //the eight directions
  static const int xdirs[] = {0, 1, 1, 1, 0, -1, -1, -1};
  static const int ydirs[] = {-1, -1, 0, 1, 1, 1, 0, -1};
  for (i = 0; i < HEIGHT; i++) {
    int x = startx, y = starty, numInARow = 0;
    for (j = 0; j < DEPTH && IsWithinBoard(x, y); j++) {
      // Test this cell here, maybe increment numInARow
      if (numInARow == 5) return TRUE;
      if (board[x][y] == 'x') numInARow++;
      x += xdirs[i];
      y += ydirs[i];
    }
  }

  return FALSE;
}

int IsWithinBoard(int x, int y) {
  return (x < 8 && x >= 0 && y < 8 && y >= 0);
}

void find_starts(char **board) {
  int i, j, k = 0, l = 0;

  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < DEPTH; j++) {
      if (board[i][j] == 'x') {
        start_pos_x[k] = i;
        start_pos_y[l] = j;
        ++k;
        ++j;
      }
    }
  }
}
