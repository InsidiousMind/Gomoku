#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../lib/gips.h"
#include "../lib/network.h"
#include "../lib/glogic.h"
#include "game_thread.h"

void *subserver(void *args); //starts subserver
int gameLoop(int reply_sock_fd, char pid);
char **addMove(char move_x, char move_y, char pid, char **board);
int turn();
void sendOtherPlayerGIPS(char pid, char other_pid, int sockfd);
int checkWin(char **board, char pid, int sockfd);
char getThisPlayersPID(int client_count);
char getOtherPlayersPID(char pid);
void sendPID(char pid, int reply_sock_fd);
void isMyTurn(int *currentTurn);
void sendMoves(int reply_sock_fd, int numTurns, char pid);



//starts each parallel thread, as programmed in game_thread.c
void start_subserver(int reply_sock_fd[2]){

  game gameInfo; 
  pthread_t pthread;
  pthread_t pthread2;


  //make the thread detached
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  
  gameInfo.args.socket = reply_sock_fd[0];
 
  gameInfo.args.socket2 = reply_sock_fd[1];
  //lock global var. Whoever gets to it first is player 1. that person sets it to TRUE 
  
  pthread_mutex_init(&gameInfo.gameInfo_access, NULL);
  //start two threads, one for each client 
  gameInfo.player1Taken = FALSE;
  
  if (pthread_create(&pthread, &attr, (void *) subserver, (void *) &gameInfo) != 0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

  if (pthread_create(&pthread2, &attr, (void *) subserver, (void *) &gameInfo) != 0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

}
///\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
//START OF THREAD
///\/\/\/\//\/\/\/\/\/\//\\/\/\/\/\

void *subserver(void *arguments) {
  //get the arguments
  char pid;
  int reply_sock_fd; 
  game *gameInfo = arguments;
  
  pthread_mutex_t gameInfo_access = gameInfo->gameInfo_access;
  
  pthread_mutex_lock(&gameInfo_access);
    if(gameInfo->player1Taken == FALSE){
      pid = 1;
      reply_sock_fd = gameInfo->args.socket;
      gameInfo->player1Taken = TRUE;
    }else{
      pid = 2;
      reply_sock_fd = gameInfo->args.socket2;
    }
  pthread_mutex_unlock(&gameInfo_access);

 
  gips *player_info;

  int read_count = -1;
  int win;

  int BUFFERSIZE = 256;
  char *buffer = calloc(BUFFERSIZE, sizeof(char));


  printf("subserver ID = %lu\n", (unsigned long) pthread_self());

  read_count = recv(reply_sock_fd, buffer, BUFFERSIZE, 0);
  buffer[read_count] = '\0';
  printf("%s\n", buffer);

  if ((win = gameLoop(reply_sock_fd, pid)) == -1) {
    perror("[!!!] error: Game Loop Fail");
  }

  if ((win == 1 && pid == 1) || (win == 2 && pid == 2))
    send_mesg("You Win! :-)\x00", reply_sock_fd);
  else {
    player_info = pack(pid, TRUE, -1, -1);
    send_to(player_info, reply_sock_fd);
    send_mesg("You Lose :-(\x00", reply_sock_fd);
  }
  close(reply_sock_fd);
  free(buffer);
  pthread_exit(NULL);

}


/*This is where the magic happens, conversation between client->server server->client
*/
int gameLoop(int reply_sock_fd, char pid) {
  
  int i, isWin, numTurns = 0, currentTurn = 1;

  //initialize and calloc board
  char **playerBoard = calloc(HEIGHT, sizeof(char *));
  for (i = 0; i < HEIGHT; i++) {
    playerBoard[i] = calloc(DEPTH, sizeof(char));
  }
  gips *player_info = calloc(sizeof(gips), sizeof(gips));

  //variables to keep track of the other player

  sendPID(pid, reply_sock_fd);

  int read_count = -1;


  do {
      //wait until other players turn is over,
      //can't play the game all at once!
    while (currentTurn != pid){
      isMyTurn(&currentTurn);
      if(currentTurn == 0) return 0;
      sleep(1);
    }

    //send other players moves
    sendMoves(reply_sock_fd, numTurns, pid);

    read_count = recv(reply_sock_fd, player_info, sizeof(player_info), 0);

    //add the move to the board, and to the respective client arrays keeping track of
    //each players moves
    playerBoard = addMove(player_info->move_a, player_info->move_b,
        player_info->pid, playerBoard);

    isWin = checkWin(playerBoard, pid, reply_sock_fd);
    //switch the turn global var and set currentTurn to it
    currentTurn = turn();
    numTurns++;
  } while (isWin == 0);

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
void sendMoves(int reply_sock_fd, int numTurns, char pid){
  char otherPID = getOtherPlayersPID(pid);

  if(numTurns == 0 && pid == 1)
    send_to(pack(otherPID, FALSE, -1,-1), reply_sock_fd);
  else
    sendOtherPlayerGIPS(pid, otherPID, reply_sock_fd);
}

//send OTHER players moves
//send other PID
//send  players turn
void sendOtherPlayerGIPS(char pid, char otherPID, int sockfd) {
  
  if (pid == 1) {

    //pack a gips player with turns of other player, other players pid, current turn,
    pthread_mutex_lock(&play2Moves_access);
    send_to(pack(otherPID, FALSE, (char) play2Moves[0], (char) play2Moves[1]), sockfd);
    pthread_mutex_unlock(&play2Moves_access);

  } else {

    pthread_mutex_lock(&play1Moves_access);
    send_to(pack(otherPID, FALSE, (char) play1Moves[0], (char) play1Moves[1]), sockfd);
    pthread_mutex_unlock(&play1Moves_access);

  }
}

//checks for a win using "check for win" in glogic library
int checkWin(char **board, char pid, int sockfd) {
 
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
    pthread_mutex_lock(&playerWin_access);
    playerWin = npid;
    pthread_mutex_unlock(&playerWin_access);

    return pid;

  } else {

    send(sockfd, &noWin, sizeof(int), 0);

    return 0;

  }
}


char **addMove(char move_a, char move_b, char pid, char **board) {

  if (pid == 1) {
    board[(int) move_a][(int) move_b] = 'x';
    pthread_mutex_lock(&play1Moves_access);
    play1Moves[0] = (int) move_a;
    play1Moves[1] = (int) move_b;
    pthread_mutex_unlock(&play1Moves_access);
  } else if (pid == 2) {
    board[(int) move_a][(int) move_b] = 'x';
    pthread_mutex_lock(&play2Moves_access);
    play2Moves[0] = (int) move_a;
    play2Moves[1] = (int) move_b;
    pthread_mutex_unlock(&play2Moves_access);
  }
  return board;

}

//checks if it is this
//make sure turn logic works out
//set whoTurn
int turn() {
  int tempTurn;

  pthread_mutex_lock(&whoTurn_access);
  tempTurn = whoTurn;
  pthread_mutex_unlock(&whoTurn_access);


  if (tempTurn == 1) {
    pthread_mutex_lock(&whoTurn_access);
    whoTurn = 2;
    pthread_mutex_unlock(&whoTurn_access);
    return 2;
  } else {
    pthread_mutex_lock(&whoTurn_access);
    whoTurn = 1;
    pthread_mutex_unlock(&whoTurn_access);
    return 1;
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
  if (pid == 1)
    send(reply_sock_fd, &pid, sizeof(char), 0);
  else
    send(reply_sock_fd, &pid, sizeof(char), 0);
}

//function to check for turns
void isMyTurn(int *currentTurn){
    
  pthread_mutex_lock(&whoTurn_access);
    (*currentTurn) = whoTurn;
    pthread_mutex_unlock(&whoTurn_access);

    pthread_mutex_lock(&playerWin_access);
    if (playerWin != 0) {
      pthread_mutex_unlock(&playerWin_access);
      (*currentTurn) = 0;
      return;
    }
    pthread_mutex_unlock(&playerWin_access);
}

