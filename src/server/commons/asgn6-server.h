#include "../../lib/database.h"

#ifndef BACKLOG
#define BACKLOG 10
#endif
#ifndef NUM_THREADS
#define NUM_THREADS 2
#endif
#ifndef ASGN6-SERVER_H
#define ASGN6-SERVER_H
void serverLoop(int fd, Node **head, pthread_mutex_t *head_access);
int accept_client(int serv_sock);
#define ASGN6-SERVER_H
