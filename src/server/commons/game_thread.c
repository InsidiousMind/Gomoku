#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include "../../lib/gips.h"
#include "../../lib/network.h"
#include "../../lib/glogic.h"
#include "../../lib/database.h"
#include "server_db.h"
#include "game_thread.h"
#include "../../lib/IO_sighandle.h"

void *subserver(void *args); //starts subserver
int gameLoop(int reply_sock_fd, char pid, void **args);
char **addMove(char move_a, char move_b, char pid, char **board, game *gameInfo);
void turn(game *gameInfo);
void sendOtherPlayerGIPS(char pid, char otherPID, int sockfd, int play1Moves[2], int play2Moves[2], int isWin);
int checkWin(char **board, char pid, int sockfd, game *gameInfo);
char getThisPlayersPID(int client_count);
char getOtherPlayersPID(char pid);
void sendPID(char pid, int reply_sock_fd);
int isMyTurn(game *gameInfo, char pid);
void sendMoves(int reply_sock_fd, int numTurns, char pid, game *gameInfo);
int checkUPID(int *uPID, char *str);
int genUPID();


/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
//START OF GAME THREAD
///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/

//starts each parallel thread, as programmed in game_thread.c
void *startGameServer(void *args){
  
  gameArgs *gameSrvInfo = (gameArgs*)args;

  game *gameInfo = calloc(1, sizeof(game));
  
  pthread_t pthread, pthread2;

  gameInfo->args.socket = gameSrvInfo->reply_sock_fd[0];
  gameInfo->args.socket2 = gameSrvInfo->reply_sock_fd[1];
  gameInfo->args.fd = gameSrvInfo->fd;
  
  pthread_mutex_lock(&(*(gameSrvInfo->head_access)));
  gameInfo->args.head = gameSrvInfo->head;   
  pthread_mutex_unlock(&(*(gameSrvInfo->head_access)));
 
  gameInfo->args.head_access = gameSrvInfo->head_access;
  

  //create a mutex to avoid race condition for shared game resources
  pthread_mutex_init(&gameInfo->gameInfo_access, NULL);
 
  gameInfo->player1Taken = FALSE;
  gameInfo->whoTurn = 1;
  gameInfo->playerWin = FALSE;

  if (pthread_create(&pthread, NULL, (void *) subserver, (void *) gameInfo) != 0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

  if (pthread_create(&pthread2, NULL, (void *) subserver, (void *) gameInfo) != 0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

  signal(SIGINT, INThandle);

  pthread_join(pthread, NULL);
  pthread_join(pthread2, NULL);
  
  pthread_mutex_destroy(&gameInfo->gameInfo_access);
  free(gameInfo);

  pthread_exit(NULL);
}

/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
//END OF GAME THREAD
///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/


/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
//START OF CLIENT THREAD
///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/

void *subserver(void *arguments) {
  //get the arguments

  char PID;
  int uPID = 0;
  int reply_sock_fd, fd; 
  Node *head;
  
  game *gameInfo = ((game *) arguments);
  fd = gameInfo->args.fd; 
  pthread_mutex_lock(&(*(gameInfo->args.head_access)));
  head = gameInfo->args.head; 
  pthread_mutex_unlock(&(*(gameInfo->args.head_access)));


  pthread_mutex_t gameInfo_access = gameInfo->gameInfo_access;

  //whoever unlocks this first gets player 1!
  pthread_mutex_lock(&gameInfo_access);
  if(gameInfo->player1Taken == FALSE){
    PID = 1;
    reply_sock_fd = gameInfo->args.socket;
    gameInfo->player1Taken = TRUE;
  }else{
    PID = 2;
    reply_sock_fd = gameInfo->args.socket2;
  }
  pthread_mutex_unlock(&gameInfo_access);

  int read_count = -1;
  int win;

  printf("subserver ID = %lu\n", (unsigned long) pthread_self());

  if(recv(reply_sock_fd, &uPID, sizeof(int), 0) == -1)
    perror("[!!!] error: receive fail in subserver");

  int BUFFERSIZE = 256;
  char *username = calloc(1, BUFFERSIZE*sizeof(char));

  if((read_count = recv(reply_sock_fd, username, BUFFERSIZE, 0)) == -1)
    perror("[!!] error: receive fail in subserver");
  username[read_count] = '\0';
  printf("%s\n", username);

  //check if username and uPID match/exist
  
  if(isPlayerTaken(&gameInfo->args.head, uPID, username, fd) == TRUE){
    uPID = genUPID();
    send(reply_sock_fd, &uPID, sizeof(uPID), 0);
  }else{
    send(reply_sock_fd, &uPID, sizeof(uPID), 0);
  }

  if ((win = gameLoop(reply_sock_fd, PID, &arguments)) == -1) {
    perror("[!!!] error: Game Loop Fail");
  }
 
 
  printf("%s%d%s", "GameLoop over for uPid ", uPID, " Performing cleanup...\n");

  
  recPlayer((&(gameInfo->args.head_access)), uPID,  gameInfo->args.fd, 
             win, gameInfo->args.head, username, PID, reply_sock_fd);
  
  close(reply_sock_fd);

  free(username);
  
  pthread_exit(NULL);
}


/*This is where the magic happens, conversation between client->server server->client
*/
int gameLoop(int reply_sock_fd, char pid, void **args) {

  game *gameInfo = *((game **) args);
  int i, isWin, numTurns = 0;

  //initialize and calloc a game-board
  //this board is on a per-player basis
  char **playerBoard = calloc(HEIGHT, sizeof(char *));
  for (i = 0; i < HEIGHT; i++) {
    playerBoard[i] = calloc(DEPTH, sizeof(char));
  }

  gips *player_info = calloc(sizeof(gips), sizeof(gips));

  sendPID(pid, reply_sock_fd);

  int read_count = -1;

  //wait until other players turn is over,
  //can't play the game all at once!
  do {

    //wait until player turn
    while(isMyTurn(gameInfo, pid) != TRUE) sleep(1);
    
    //send other players moves
    sendMoves(reply_sock_fd, numTurns, pid, gameInfo);
    
    int wpid = 0;
    pthread_mutex_lock(&gameInfo->gameInfo_access);
    wpid = gameInfo->playerWin;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
    if(wpid!=0)
      break;

    if((read_count = recv(reply_sock_fd, player_info, sizeof(player_info), 0)) == -1)
      perror("[!!!] ERROR: receive error in GameLoop");
  
    //add the move to the board, and to the respective client arrays keeping track of
    //each players moves
    playerBoard = addMove(player_info->move_a, player_info->move_b,
        player_info->pid, playerBoard, gameInfo);
    
    //check for a win 
    isWin = checkWin(playerBoard, pid, reply_sock_fd, gameInfo);
   
    //switch the turn 
    turn(gameInfo);

    numTurns++;

  } while (isWin == 0 && read_count != -1 && read_count != 0);
  

  printf("%s%d%s", "Game ended for pid ", pid, " Performing cleanup...\n");

  for(i = 0; i < HEIGHT; i++){
    free(playerBoard[i]);
  }
  
  free(playerBoard);
  free(player_info);

  return isWin;

}

//check for what moves to send
//if no one has moved yet (IE player 1 to move first)
//a dummy gips packet with -1 -1 is sent
void sendMoves(int reply_sock_fd, int numTurns, char pid, game *gameInfo){

  char otherPID = getOtherPlayersPID(pid);

  if(numTurns == 0 && pid == 1)

    send_to(pack(otherPID, FALSE, -1,-1), reply_sock_fd);

  else{

    pthread_mutex_lock(&gameInfo->gameInfo_access);
    sendOtherPlayerGIPS(pid, otherPID, reply_sock_fd, gameInfo->play1Moves, gameInfo->play2Moves, gameInfo->playerWin);
    pthread_mutex_unlock(&gameInfo->gameInfo_access);

  }
}

//send OTHER players moves
//send other PID
//send  players turn
void sendOtherPlayerGIPS(char pid, char otherPID, int sockfd, int play1Moves[2], int play2Moves[2], int isWin) {


  if (pid == 1)
    send_to(pack(otherPID, (char) isWin, (char) play2Moves[0], (char) play2Moves[1]), sockfd);
  else 
    send_to(pack(otherPID, (char) isWin, (char) play1Moves[0], (char) play1Moves[1]), sockfd);
}

//checks for a win using "check for win" in glogic library
int checkWin(char **board, char pid, int sockfd, game *gameInfo) {

  int p1win = 0, p2win = 0;
  int npid = (int) pid;
  int noWin = 0;

  //call on checkForWin from glogic for subsequent PID's. 
  //have to do it seperately
  if (pid % 2 == 0) 
    p2win = check_for_win_server(board);
  else
    p1win = check_for_win_server(board);
    
   
  if ((p1win == TRUE) || (p2win == TRUE)) {
    
    send(sockfd, &npid, sizeof(int), 0);
    
    pthread_mutex_lock(&gameInfo->gameInfo_access);
    gameInfo->playerWin = npid;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
    
    return pid;
    
  } else {
    
    send(sockfd, &noWin, sizeof(int), 0);
    
    return 0;
    
  }
}


char **addMove(char move_a, char move_b, char pid, char **board, game *gameInfo) {


  pthread_mutex_lock(&gameInfo->gameInfo_access);

  if (pid == 1) {

    board[(int) move_a][(int) move_b] = 'x';
    gameInfo->play1Moves[0] = (int) move_a;
    gameInfo->play1Moves[1] = (int) move_b;

  } else if (pid == 2) {

    board[(int) move_a][(int) move_b] = 'x';
    gameInfo->play2Moves[0] = (int) move_a;
    gameInfo->play2Moves[1] = (int) move_b;

  }
  pthread_mutex_unlock(&gameInfo->gameInfo_access);

  return board;

}

//checks if it is this
//make sure turn logic works out
//set whoTurn
void turn(game *gameInfo) {

  pthread_mutex_lock(&gameInfo->gameInfo_access);

  if (gameInfo->whoTurn == 1) {
    gameInfo->whoTurn = 2;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
  } else {
    gameInfo->whoTurn = 1;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
  }
}

char getThisPlayersPID(int client_count){
  if (client_count % 2 == 0)
    return 1; 
  else
    return 2;
}

char getOtherPlayersPID(char pid){
  if (pid == 1)
    return 2;
  else
    return 1;
}

void sendPID(char pid, int reply_sock_fd){
  send(reply_sock_fd, &pid, sizeof(char), 0);
}

//function to check for turns
int isMyTurn(game *gameInfo, char pid){
  
  int currentTurn; 
  pthread_mutex_lock(&gameInfo->gameInfo_access);
  currentTurn = gameInfo->whoTurn;
  pthread_mutex_unlock(&gameInfo->gameInfo_access);

  if(currentTurn == pid)
    return TRUE;
  else
    return FALSE;
} 


//placeholder functions
int genUPID(){
  return rand() % 500; //return a random number between 0 and 499
}



/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
//END OF CLIENT THREAD
///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/



