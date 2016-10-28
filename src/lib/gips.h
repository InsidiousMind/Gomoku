#ifndef GIPS
#define GIPS
typedef struct {
	short player;
	long long move;
} gips;
gips *encode(int x, int y);
int *decode(gips *x);
void someone_won(gips *x);
#endif /*GIPS*/
