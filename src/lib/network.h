#include "gips.h"

#ifndef HOST
#define HOST "server1.cs.scranton.edu"
#endif
#ifndef HTTPPORT
#define HTTPPORT "32200"
#endif

int connect_to_server();

int send_to(gips *info, int sock);

gips *get_server(int sock);

int send_mesg(char *str, int sock);

int readBytes(int sock, unsigned int x, void *buffer);

int send_misc(void *str, int sock);

