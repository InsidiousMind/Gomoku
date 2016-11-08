#ifndef BACKLOG
#define BACKLOG 10
#endif
#ifndef NUM_THREADS
#define NUM_THREADS 2
#endif

pthread_t* server_loop(int *client_count);

