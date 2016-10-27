#include "gips.h"

// The Gomoku Inter Process Shuttle source file.

extern char *board[8][8];

void encode_send() {
	
}

void decode_recv() {
		
}

void toggle_node(int a, int b, char c) {
	board[a][b] = c;	
}
