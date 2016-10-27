#include <gips.h>

void someone_won(long x) {
	// Takes a single 0 or 1 to tell us if we won or not.
	// 1 is a loss.
	switch (x) {
		case 1:
			printf("You've lost.");
		case 0:
			printf("You win!");
	}
	exit(0);
}
