#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../lib/gips.h"
#include "../lib/network.h"
#include "../lib/glogic.h"


void start_subserver(int reply_sock_fd, int client_count); //starts subserver
void *subserver(void *args); //starts subserver
int gameLoop(int reply_sock_fd, char pid);
char **addMove(char move_x, char move_y, char pid, char **board);
int turn();
void sendMoves(char pid, char other_pid, int sockfd);
int checkWin(char **board, char pid, int sockfd);


struct game {
  // This will serve as the linked list to keep two threads connected as one
  // game. Andrew - check my syntax?
  struct game *ref;
  // pthread one;
  // pthread two;
};

typedef struct game Game;

//shouldn't need a mutext lock for these because 
//they are only accessed by their respective threads

//just for storing args which are going to be passed to pthread_create


struct moves{
  int play1Moves[2];
  int play2Moves[2];

  int whoTurn;
} pmoves;


//args to pass to game_thread
struct arg_s {
  long arg1;
  char arg2;
};


pthread_mutex_t play1Moves_access = PTHREAD_MUTEX_INITIALIZER;
int play1Moves[2];

pthread_mutex_t play2Moves_access = PTHREAD_MUTEX_INITIALIZER;
int play2Moves[2];

pthread_mutex_t whoTurn_access = PTHREAD_MUTEX_INITIALIZER;
int whoTurn;

pthread_mutex_t playerWin_access = PTHREAD_MUTEX_INITIALIZER;
int playerWin;



//starts each parallel thread, as programmed in game_thread.c
void start_subserver(int reply_sock_fd, int client_count) 
{

  struct arg_s args;
  pthread_t pthread;
  long reply_sock_fd_long = reply_sock_fd;
  args.arg1 = reply_sock_fd_long;

  if (client_count % 2 == 0)
    args.arg2 = 1;
  else if (client_count % 2 == 1)
    args.arg2 = 2;

  if (pthread_create(&pthread, NULL, (void *) subserver, (void *) &args) != 0) 
    printf("failed to start subserver\n");
  else 
    printf("subserver %lu started\n", (unsigned long) pthread);
}

void *subserver(void *arguments) {
  // TODO
  // NEED to implement a "wait" function so that if only one client is connected
  // they don't play the game themselves
  //
  // If someone connects while two clients are already connected
  // need to kill connection and tell them that two players are already playing
  //get the arguments
  struct arg_s *args = arguments;
  long reply_sock_fd_long = args->arg1;
  char pid = args->arg2;
  gips *player_info;

  int read_count = -1;
  int win;
  
  int BUFFERSIZE = 256;
  char buffer[BUFFERSIZE + 1];

  int reply_sock_fd = (int) reply_sock_fd_long;

  printf("subserver ID = %lu\n", (unsigned long) pthread_self());

  read_count = readBytes(reply_sock_fd, BUFFERSIZE, &buffer);
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
  return NULL;

}


/*This is where the magic happens, once the client and server
 * all this function does so far is keep receiving stuff it gets from
 * the client and keep checking for a win
 *
 * TODO: send back the other players move
 *   this requires: a shared resource with the other thread to determine
 *   the other players move, and send it back to this specific subservers
 *   client so that the client can update the gameboard
 */
int gameLoop(int reply_sock_fd, char pid) {
  int isWin;
  int i;

  char **playerBoard = calloc(HEIGHT, sizeof(char *));
  for (i = 0; i < HEIGHT; i++) {
    playerBoard[i] = calloc(DEPTH, sizeof(char));
  }

  //for the first time, white goes first (Player 2)
  gips *player_info;
  int currentTurn;
  //make variables to keep track of the other player

  char other_pid;
  if (pid == 1) {
    other_pid = 2;
    pthread_mutex_lock(&play1Moves_access);
    play1Moves[0] = 3;
    play1Moves[1] = 3;
    pthread_mutex_unlock(&play1Moves_access);

    playerBoard[3][3] = 'x';
  } else if (pid == 2) {
    other_pid = 1;
  }

  //game starts with player2's turn, player 1 automatically starts at 3,3
  //                                                 (middle of the board)

  pthread_mutex_lock(&whoTurn_access);
  whoTurn = 2;
  currentTurn = whoTurn;
  pthread_mutex_unlock(&whoTurn_access);

  //first packet sent to respective client concerns
  //the own players board, every packet after that is
  //about the OTHER players board
  if (pid == 1)
    player_info = pack(pid, FALSE, 3, 3);
  else
    player_info = pack(pid, FALSE, 3, 3);

  //send the first instantiated game board with
  //player1 moved on a center piece
  send_to(player_info, reply_sock_fd);

  int read_count = -1;


  do {
    //receive board of client we are conversing with


    //Waiting: if it's not the clients turn, the server will send a gips packet with
    //the clients own PID and waiting set to 1 (true).
    //while the client is waiting, it should keep itself in a loop, while printing to the user
    //that the other player is making a move
    //once the client receives a packer with waiting set to FALSE then that gips packet
    //will contain the other players moves
    //go and move if it's the clients turn
    while (currentTurn != pid) {

      pthread_mutex_lock(&whoTurn_access);
      currentTurn = whoTurn;
      pthread_mutex_unlock(&whoTurn_access);

      pthread_mutex_lock(&playerWin_access);
      if (playerWin != 0) {
        pthread_mutex_unlock(&playerWin_access);
        return 0;
      }
      pthread_mutex_unlock(&playerWin_access);
      sleep(1);
    }

    //send other players moves and check for win
    sendMoves(pid, other_pid, reply_sock_fd);

    read_count = readBytes(reply_sock_fd, sizeof(player_info), player_info);

    //add the move to the board, and to the respective client arrays keeping track of
    //each players moves
    playerBoard = addMove(player_info->move_a, player_info->move_b,
        player_info->pid, playerBoard);

    isWin = checkWin(playerBoard, pid, reply_sock_fd);

    //if it's a win, change the turn and return from the gameLoop
    if (isWin != 0) {
      return isWin;
    }

    //switch the turn global var and set currentTurn to it
    currentTurn = turn();


  } while (read_count != 0 || read_count != -1);

  for (i = 0; i < HEIGHT; i++) {
    free(playerBoard[i]);
  }
  free(playerBoard);

  return read_count == -1 ? -1 : 0; //-1 on fail 0 on success

}

//send OTHER players moves
//send other PID
//send  players turn
void sendMoves(char pid, char other_pid, int sockfd) {
  gips *other_player;
  if (pid == 1) {
    //pack a gips player with turns of other player, other players pid, current turn,
    pthread_mutex_lock(&play2Moves_access);
    other_player = pack(other_pid, FALSE, (char) play2Moves[0], (char) play2Moves[1]);
    pthread_mutex_unlock(&play2Moves_access);
  } else {
    pthread_mutex_lock(&play1Moves_access);
    other_player = pack(other_pid, FALSE, (char) play1Moves[0], (char) play1Moves[1]);
    pthread_mutex_unlock(&play1Moves_access);
  }

  send_to(other_player, sockfd);

}

//checks for a win using "check for win" in glogic library
int checkWin(char **board, char pid, int sockfd) {
  int p1win = 0, p2win = 0;
  int npid = (int) pid;
  int noWin = 0;

  //most up-to-date moves are this player
  if (pid % 2 == 0) {
    p2win = check_for_win_server(board);
  } else {
    p1win = check_for_win_server(board);
  }

  if ((p1win == TRUE) || (p2win == TRUE)) {
    send_misc(&npid, sockfd);
    //send(sockfd, &npid, sizeof(int), 0);
    pthread_mutex_lock(&playerWin_access);
    playerWin = npid;
    pthread_mutex_unlock(&playerWin_access);
    return pid;
  } else {
    send_misc(&noWin, sockfd);
    //send(sockfd, &noWin, sizeof(int), 0);
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
