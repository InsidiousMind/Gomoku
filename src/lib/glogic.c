#include "glogic.h"
#include "gips.h"

void check_for_win_server(gips *x, char **board) {
	// Set x->is_win to 0 if nobody won, otherwise set it to the
	// Player number of the winner.
	
  x->isWin = crawl_board(board);

}

char crawl_board(char **board) {
	/*
	 * In C, chars are just ascii values stored in a byte of memory.
	 * We don't need a full integer, so we're going to leverage the
	 * manner in which chars are represented to provide us with a 
	 * smaller memory footprint.
	 */
	char i;
	char j;
	char k;
	char l;
	for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                k = -5;
                l = -5;
                while (k < 6) {
                    while (l < 6) {
                        // We're going to make sure we aren't overflowing the board.
                        if ((((i + k < 8) || (j + l < 8)) 
                            || ((i + k >= 0) || (j + l >= 0)))
                            || ()) { // This needs to check to make sure we're only checking valid combinations.
                            continue;
                        }
                        // Here's where we check if we have a win on the board.
                        
                        // Next.
                        l++;
                    }
                    k++;
                }
            }
        }
}
