#ifndef ARGS
#define ARGS
//args to pass to game_thread
typedef struct pthread_args {
  int socket;
  int socket2;
} pargs;
#endif /* ARGS */

#ifndef GAME
#define GAME
//keep track of games and game information
typedef struct game_s {
  
  pthread_mutex_t gameInfo_access;

  //shared variables 
  int play1Moves[2];
  int play2Moves[2];
  int whoTurn;
  int playerWin;
  int player1Taken;
   
  pargs args;

  struct game *next;
} game;
#endif /* GAME */


void start_subserver(int reply_sock_fd[2]); //starts subserver
