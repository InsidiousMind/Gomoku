#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "glogic.h"
#include "gips.h"


int start_pos_x[HEIGHT];
int start_pos_y[DEPTH];

void find_starts(char **board);

int IsWithinBoard(int x, int y);

int crawl_board(char **board, int startx, int starty);


int main() {


    char **board = malloc(HEIGHT * sizeof(char *));
    for (int i = 0; i < HEIGHT; i++) {
        board[i] = malloc(DEPTH);
    }
    board[3][3] = 'x';
    board[2][2] = 'x';
    board[3][4] = 'x';

    printf("%d\n", check_for_win_server(board));

}

int check_for_win_server(char **board) {
    int isWin = 0;
    find_starts(board);

    // Set x->is_win to 0 if nobody won, otherwise set it to the
    // Player number of the winner.
    //
    for (int i = 0; i < (sizeof(start_pos_x) / sizeof(char)); i++) {
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
    int j;                    //the eight directions
    static const int xdirs[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static const int ydirs[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    for (i = 0; i < HEIGHT; i++) {
        int x = startx, y = starty, numInARow = 0;
        for (j = 0; j < DEPTH && IsWithinBoard(x, y); j++) {
            // Test this cell here, maybe increment numInARow 
            if (board[x][y] == 'x') numInARow++;
            if (numInARow == 5) return TRUE;
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
    int k = 0, l = 0;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < DEPTH; j++) {
            if (board[i][j] == 'x') {
                start_pos_x[k] = i;
                start_pos_y[l] = j;
                ++k;
                ++j;
            }
        }
    }
}

