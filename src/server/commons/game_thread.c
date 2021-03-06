#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
//shared libraries
#include "../../lib/gips.h"
#include "../../lib/glogic.h"
#include "../../lib/database.h"

//commons
#include "server_db.h"
#include "game_thread.h"
#include "server_connections.h"

void *subserver(void *args); //starts subserver
int gameLoop(int reply_sock_fd, char pid, void **args);

char **
addMove(char move_a, char move_b, char pid, char **board, game *gameInfo);

void turn(game *gameInfo);

int checkWin(char **board, char pid, int sockfd, game *gameInfo);

char getOtherPlayersPID(char pid);

int sendPID(char pid, int reply_sock_fd);

bool isMyTurn(game *gameInfo, char pid);

int sendMoves(int reply_sock_fd, int numTurns, char pid, game *gameInfo);

int genUPID();

void earlyExit(BYTE PID, char **username, int reply_sock_fd, game **gameInfo);

int c_exit(game **gameInfo, BYTE PID, char **username, int reply_sock_fd);

int otherClientDisconnected(game **gameInfo, BYTE PID, char **username,
                            int reply_sock_fd);

/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
  //START OF GAME THREAD
  ///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/

//starts each parallel thread, as programmed in game_thread.c
void *startGameServer(void *args) {

  gameArgs *gameSrvInfo = (gameArgs *) args;

  game *gameInfo = calloc(1, sizeof(game));

  pthread_t pthread, pthread2;
  pthread_mutex_t conn_head_access = gameSrvInfo->conn_head_access;

  gameInfo->args.socket = gameSrvInfo->reply_sock_fd[0];
  gameInfo->args.socket2 = gameSrvInfo->reply_sock_fd[1];
  gameInfo->args.fd = gameSrvInfo->fd;
  gameInfo->args.uPID1 = 0;
  gameInfo->args.uPID2 = 0;
  gameInfo->args.head = gameSrvInfo->head;
  gameInfo->args.head_access = *gameSrvInfo->head_access;


  //create a mutex to avoid race condition for shared game resources
  pthread_mutex_init(&gameInfo->gameInfo_access, NULL);

  gameInfo->player1Taken = FALSE;
  gameInfo->whoTurn = 1;
  gameInfo->playerWin = FALSE;

  if (pthread_create(&pthread, NULL, (void *) subserver, (void *) gameInfo) !=
      0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

  if (pthread_create(&pthread2, NULL, (void *) subserver, (void *) gameInfo) !=
      0)
    perror("failed to start subserver\n");
  else
    printf("subserver %lu started\n", (unsigned long) pthread);

  pthread_join(pthread, NULL);
  pthread_join(pthread2, NULL);
  pthread_mutex_lock(&conn_head_access);
  parseConnections(&gameSrvInfo->conn_head);
  pthread_mutex_unlock(&conn_head_access);

  pthread_mutex_destroy(&gameInfo->gameInfo_access);

  free(gameInfo);
  printf("Threads Exited Successfully :D\n");
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
  BYTE PID;
  uint32_t uPID = 0;
  int reply_sock_fd, fd;
  Node *head;

  game *gameInfo = ((game *) arguments);

  fd = gameInfo->args.fd;

  head = gameInfo->args.head;

  pthread_mutex_t gameInfo_access = gameInfo->gameInfo_access;

  //whoever unlocks this first gets player 1!
  pthread_mutex_lock(&gameInfo_access);
  if (gameInfo->player1Taken == false) {
    PID = 1;
    reply_sock_fd = gameInfo->args.socket;
    gameInfo->player1Taken = true;
  } else {
    PID = 2;
    reply_sock_fd = gameInfo->args.socket2;
  }
  pthread_mutex_unlock(&gameInfo_access);

  ssize_t read_count;

  int win;

  printf("subserver ID = %lu\n", (unsigned long) pthread_self());
  //login receives
  //first packet twe receive is the clients 'Expected' unique PID
  //receives the unique PID
  if ((read_count = recv(reply_sock_fd, &uPID, sizeof(int), 0)) == -1)
    perror("[!!!] error: receive fail in subserver");
  uPID = ntohl(uPID);
  if (read_count == 0) c_exit(&gameInfo, PID, NULL, reply_sock_fd);
  //next packet is the clients Username
  size_t BUFFERSIZE = 256;
  char *username = calloc(1, BUFFERSIZE * sizeof(char));
  if ((read_count = recv(reply_sock_fd, username, BUFFERSIZE, 0)) == -1)
    perror("[!!] error: receive fail in subserver");

  if (read_count == 0) c_exit(&gameInfo, PID, &username, reply_sock_fd);

  username[read_count] = '\0';

  //check if username and uPID match/exist
  //if they don't, send the player a uniquePID
  int send_ret = 0;
  pthread_mutex_lock(&gameInfo->args.head_access);
  if (isPlayerTaken(&head, uPID, username, fd) == true) {
    uPID = (uint32_t) genUPID();
    uPID = htonl(uPID);
    send_ret = send(reply_sock_fd, &(uPID), sizeof(uPID), 0);
    uPID = ntohl(uPID);
  } else {
    uPID = htonl(uPID);
    send_ret = send(reply_sock_fd, &uPID, sizeof(uPID), 0);
    uPID = ntohl(uPID);
  }
  pthread_mutex_unlock(&gameInfo->args.head_access);

  //assign playerID's
  uint32_t other_uPID;
  if (PID == 1) {
    gameInfo->args.uPID1 = uPID;
  } else {
    gameInfo->args.uPID2 = uPID;
  }

  //sync up the threads
  while (gameInfo->args.uPID1 == 0 || gameInfo->args.uPID2 == 0) {
    sleep(1);
  }

  //get the other players uPID
  pthread_mutex_lock(&gameInfo_access);
  if (PID == 1) {
    other_uPID = gameInfo->args.uPID2;
  } else {
    other_uPID = gameInfo->args.uPID1;
  }
  pthread_mutex_unlock(&gameInfo_access);

  if (send_ret == -1) c_exit(&gameInfo, PID, &username, reply_sock_fd);

  if ((sendPID(PID, reply_sock_fd) == -1))
    c_exit(&gameInfo, PID, &username, reply_sock_fd);

  pthread_mutex_lock(&(gameInfo->args.head_access));
  //send player stats
  int ret_sendp = sendPlayer(uPID, username, head, reply_sock_fd, fd);
  //record player in database(if new)
  recPlayer(uPID, PID, username, -1, head, fd);
  pthread_mutex_unlock(&(gameInfo->args.head_access));
  //kill thread if client exited
  if (ret_sendp == -1) c_exit(&gameInfo, PID, &username, reply_sock_fd);

  //wait until the other thread has recorded their player in the db
  Player *otherPlay = calloc(1, sizeof(Player));
  while (otherPlay == NULL) {
    sleep(1);
    pthread_mutex_lock(&gameInfo->args.head_access);
    otherPlay = fpuPID(other_uPID, fd, &gameInfo->args.head);
    pthread_mutex_unlock(&gameInfo->args.head_access);
  }

  //send the other player
  pthread_mutex_lock(&(gameInfo->args.head_access));
  ret_sendp = sendPlayer(other_uPID, otherPlay->username, head, reply_sock_fd,
                         fd);
  pthread_mutex_unlock(&(gameInfo->args.head_access));

  if (otherPlay != NULL) free(otherPlay);
  if (ret_sendp == -1) c_exit(&gameInfo, PID, &username, reply_sock_fd);

  printf("%s %s %d %s %d\n", username, "with PID:", PID, "and uPID: ", uPID);

  if (((win = gameLoop(reply_sock_fd, PID, &arguments)) == -1)) {
    perror("[!!!]: Game Loop, client disconnect? : ");
    c_exit(&gameInfo, PID, &username, reply_sock_fd);
  }

  //in the future could have subserver return with win and record player in GameServer removing
  printf("%s%d%s", "GameLoop over for uPid ", uPID, " Performing cleanup...\n");

  Player **player;
  //TODO possible failures: recPlayer
  pthread_mutex_lock(&(gameInfo->args.head_access));
  player = recPlayer(uPID, PID, username, win, head, fd);
  pthread_mutex_unlock(&(gameInfo->args.head_access));

  //send player after recording it
  if (sendp(player, reply_sock_fd) == -1)
    c_exit(&gameInfo, PID, &username, reply_sock_fd);

  close(reply_sock_fd);

  free(username);

  pthread_exit(NULL);
}

//when the client socket has shutdown
int c_exit(game **gameInfo, BYTE PID, char **username, int reply_sock_fd) {
  game *tempInfo = *gameInfo;

  printf("Client Disconnect\n");

  pthread_mutex_lock(&tempInfo->gameInfo_access);
  if (!tempInfo->clientDisconnect) {
    tempInfo->clientDisconnect = true;
    printf("%s %d %s\n", "Client:", PID, "exited first");
  } else {
    printf("%s %d %s\n", "Client:", PID, "exited second");
  }
  pthread_mutex_unlock(&tempInfo->gameInfo_access);

  earlyExit(PID, &(*username), reply_sock_fd, &tempInfo);
  return 0;
}

void earlyExit(BYTE PID, char **username, int reply_sock_fd, game **gameInfo) {
  game *tempInfo = *gameInfo;
  char *temp = username ? *username : "NA";
  printf("%s: %s %s %c %s\n", "Player", temp, "of", (char) PID,
         "client socket closed");
  close(reply_sock_fd);
  if (username) free(temp);
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

  int read_count;
  bool clientDC;
  //wait until other players turn is over,
  //can't play the game all at once!
  do {
    //wait until player turn
    while ((isMyTurn(gameInfo, pid)) != true) sleep(1);
    pthread_mutex_lock(&gameInfo->gameInfo_access);
    clientDC = gameInfo->clientDisconnect;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);

    if (sendMoves(reply_sock_fd, numTurns, pid, gameInfo) == -1) {
      for (i = 0; i < HEIGHT; i++) {
        free(playerBoard[i]);
      }
      free(playerBoard);
      free(player_info);
      return -1;
    }

    //check for win
    int wpid = 0;
    pthread_mutex_lock(&gameInfo->gameInfo_access);
    wpid = gameInfo->playerWin;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
    if (wpid != 0)
      break;

    if (!clientDC) {
      if ((read_count = receive_gips(reply_sock_fd, &player_info)) == -1) {
        perror("[!!!] ERROR: receive error in GameLoop");
        return -1;
      }
      if (read_count == 0) {
        return -1;
      }
      //add the move to the board, and to the respective client arrays keeping track of
      //each players moves
      playerBoard = addMove(player_info->move_a, player_info->move_b,
                            player_info->pid, playerBoard,
                            gameInfo);
      //check for a win
      isWin = checkWin(playerBoard, pid, reply_sock_fd, gameInfo);
    }

    //switch the turn
    turn(gameInfo);

    numTurns++;

    //while client has not disconnected/won
  } while (isWin == 0 && read_count != -1 && read_count != 0 && !clientDC);

  printf("%s%d%s", "Game ended for pid ", pid, " Performing cleanup...\n");

  for (i = 0; i < HEIGHT; i++) {
    free(playerBoard[i]);
  }
  free(playerBoard);
  free(player_info);

  if (clientDC) return -1;
  else return isWin;
}

//check for what moves to send
//if no one has moved yet (IE player 1 to move first)
//a dummy gips packet with -1 -1 is sent
int sendMoves(int reply_sock_fd, int numTurns, char pid, game *gameInfo) {

  char otherPID = getOtherPlayersPID(pid);
  BYTE win = 0;
  if (numTurns == 0 && pid == 1) {

    pthread_mutex_lock(&gameInfo->gameInfo_access);
    if (gameInfo->clientDisconnect) win = -1;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);

    if (send_to(pack(otherPID, FALSE, -1, -1, win), reply_sock_fd) == -1) {
      printf(
          "Could not send; Other Client Disconnected or the Pipe is Broken\n");
      return -1;
    }

  } else {
    int send_ret = 0;
    pthread_mutex_lock(&gameInfo->gameInfo_access);
    if (gameInfo->clientDisconnect) win = -1;

    send_ret = send_to(pack(otherPID,
                            (BYTE) gameInfo->playerWin,
                            (BYTE) (pid == 1 ? gameInfo->play2Moves[0]
                                             : gameInfo->play1Moves[0]),
                            (BYTE) (pid == 1 ? gameInfo->play2Moves[1]
                                             : gameInfo->play1Moves[1]),
                            win),
                       reply_sock_fd);
    pthread_mutex_unlock(&gameInfo->gameInfo_access);
    if (send_ret == -1) return -1;

  }
  return 0;
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

  if ((p1win == true) || (p2win == true)) {

    if (send(sockfd, &npid, sizeof(int), 0) == -1) {
      return -1;
    }

    pthread_mutex_lock(&gameInfo->gameInfo_access);
    gameInfo->playerWin = npid;
    pthread_mutex_unlock(&gameInfo->gameInfo_access);

    return pid;

  } else if (send(sockfd, &noWin, sizeof(int), 0) == -1) {
    return -1;
  } else return 0;
}

char **
addMove(char move_a, char move_b, char pid, char **board, game *gameInfo) {

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
  } else {
    gameInfo->whoTurn = 1;
  }
  pthread_mutex_unlock(&gameInfo->gameInfo_access);
}

char getOtherPlayersPID(char pid) {
  if (pid == 1)
    return 2;
  else
    return 1;
}

int sendPID(char pid, int reply_sock_fd) {
  if (send(reply_sock_fd, &pid, sizeof(char), 0) == -1) {
    return -1;
  }
  return 0;
}

//function to check for turns
bool isMyTurn(game *gameInfo, char pid) {

  int currentTurn, clientDC;

  pthread_mutex_lock(&gameInfo->gameInfo_access);
  currentTurn = gameInfo->whoTurn;
  clientDC = gameInfo->clientDisconnect;
  pthread_mutex_unlock(&gameInfo->gameInfo_access);

  if (currentTurn == pid || clientDC) return true;
  else return 0;
}

//placeholder functions
int genUPID() {
  return rand() % 500; //return a random number between 0 and 499
}



/*/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\
  //END OF CLIENT THREAD
  ///\/\/\/\//\/\/\/\/\/\//\\/\/\/\*/



