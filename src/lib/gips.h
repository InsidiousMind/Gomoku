#ifndef GIPS
#define GIPS

typedef unsigned char BYTE;

typedef struct {
  BYTE pid;
  BYTE isWin;
  BYTE move_a;
  BYTE move_b;
} gips;

gips *pack(BYTE pid, BYTE isWin, BYTE move_x, BYTE move_y);

int send_to(gips *info, int sock);

int send_mesg(char *str, int sock);

int send_misc(void *str, int sock);

#endif /*GIPS*/

#ifndef DEPTH
#define DEPTH 8
#endif
#ifndef HEIGHT
#define HEIGHT 8
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef HOST
//#define HOST "server1.cs.scranton.edu"
//#define HOST "10.31.226.161"
#define HOST "127.0.0.1"
#endif
#ifndef HTTPPORT
#define HTTPPORT "32200"
#endif


