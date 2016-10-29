#include "glogic.h"
#include "gips.h"

void check_for_win_server(gips *x) {
	// Set x->is_win to 0 if nobody won, otherwise set it to the
	// Player number of the winner..
	char *board = unpack(x);
	x->is_win = crawl_board(board);
}

short crawl_board(char *board) {
	struct node {
		short player;
		struct node child0;
		struct node child1;
		struct node child2;
		struct node child3;
		struct node child4;
		struct node child5;
		struct node child6;
		struct node child7;
	};
	
	typedef struct node Node;
}
