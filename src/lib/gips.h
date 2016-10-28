#ifndef GIPS
#define GIPS
typedef struct {
	short player;
	long long pos;
} gips;

gips *pack(char **pos, short player); //packs long long
char **unpack(gips *info);
void someone_won(gips *x);
#endif /*GIPS*/

#ifndef DEPTH
#define DEPTH 8
#endif
#ifndef HEIGHT
#define HEIGHT 8 
#endif
