#include "glogic.h"
#include "gips.h"

void check_for_win_server(gips *x) {
	// Return 1 if yes, 0 if no.
	char *board = unpack(x);
	x->is_win = 0;
}

int check_for_win_client(gips *x) {
	if (x->is_win == 1) {
		return 1;
	} else {
		return 0;
	}
}
