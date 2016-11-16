#ifndef ARGS
#define ARGS

//arguments for gameServer
typedef struct game_srv_args
{
  int *reply_sock_fd; 
  int fd;
  Node *head;

} gameArgs;

//args to pass to game_thread
typedef struct pthread_args {
  int socket;
  int socket2;
  Node *head;
  int fd;
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

} game;
#endif /* GAME */


void *startGameServer(void *args); //starts subserver
