#include "glogic.h"
#include "gips.h"

void check_for_win_server(gips *x) {
	// Set x->is_win to 0 if nobody won, otherwise set it to the
	// Player number of the winner.
	char *board = unpack(x);
	x->is_win = crawl_board(board);
}

char crawl_board(char *board) {
	/*
	 * In C, chars are just ascii values stored in a byte of memory.
	 * We don't need a full integer, so we're going to leverage the
	 * manner in which chars are represented to provide us with a 
	 * smaller memory footprint.
	 */
	char i = 0;
	char j = 0;
	char k = 0;
	char l = 0;
	while (i < 8) {
		while (j < 8) {
			while ((i + k >= 0) && (i + k < 8)) {
				while ((j + l >= 0) && (j + l < 8)) {
					l++;
				}
				k++;
			}
			j++;
		}
		i++;
	}
}
