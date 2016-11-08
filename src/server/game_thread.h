#ifndef GAME
#define GAME

//keep track of games and game information
typedef struct game_s {
  int play1Moves[2];
  int play2Moves[2];
  int whoTurn;
  struct game *ref;
  // pthread one;
  // pthread two;
} game;
#endif /* GAME */

#ifndef ARGS
#define ARGS
//args to pass to game_thread
typedef struct pthread_args {
  int socket;
  char pid;
} pargs;
#endif /* ARGS */



pthread_t start_subserver(int reply_sock_fd, int client_count); //starts subserver
