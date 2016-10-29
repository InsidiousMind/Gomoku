#include "glogic.h"
#include "gips.h"

void check_for_win_server(gips *x) {
	// Return 1 if yes, 0 if no.
	char *board = unpack(x);
	x->is_win = crawl_board(board);
}

int check_for_win_client(gips *x) {
	// We may not need this after all...
	return 0;
}

