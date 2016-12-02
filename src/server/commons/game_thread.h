#ifndef ARGS
#define ARGS

//arguments for gameServer
typedef struct game_srv_args
{
  int reply_sock_fd[2];
  int fd;
  Node *head;
  pthread_mutex_t *head_access;

} gameArgs;

//args to pass to game_thread
typedef struct pthread_args {
  int socket;
  int socket2;
  int fd;
  Node *head;
  pthread_mutex_t head_access;
} pargs;

//keep track of games and game information
typedef struct game_s {
  
  pthread_mutex_t gameInfo_access;

  //shared variables 
  int play1Moves[2];
  int play2Moves[2];
 
  int whoTurn;
  int playerWin;
  int player1Taken;
  bool clientDisconnect;

  pargs args;

} game;

#endif /* ARGS */

void *startGameServer(void *args); //starts subservers
