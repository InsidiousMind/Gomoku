#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "../lib/gips.h"
#include "../lib/network.h"
#include "../lib/glogic.h"

#define BACKLOG 10
#define NUM_THREADS 2


void *get_in_addr(struct sockaddr * sa); //get info of incoming addr in struct
void print_ip( struct addrinfo *ai); //prints IP
void *subserver(void *args); //starts subserver, sends HTML page
int get_server_socket(char *hostname, char *port); //get a socket and bind to it
int start_server(int serv_socket, int backlog);  //starts listening on port for inc connections
int accept_client(int serv_sock); //accepts incoming connection
void start_subserver(int reply_sock_fd, int client_count); //starts subserver
int gameLoop(int reply_sock_fd, char pid);
void addMove(char move_x, char move_y, char pid);
char **initBoard(char **board);
int turn(gips *info);
int * findOtherMoves(gips *player_info);


//shouldn't need a mutext lock for these because they are only accessed by their respective threads

char **p1board;
char **p2board;
//just for storing args which are going to be passed to pthread_create

pthread_mutex_t play1Moves = PTHREAD_MUTEX_INITIALIZER;
int play1[2];
pthread_mutex_t play2Moves = PTHREAD_MUTEX_INITIALIZER;
int play2[2];

pthread_mutex_t whoTurn_access = PTHREAD_MUTEX_INITIALIZER;
int whoTurn;

struct arg_s{
  long arg1;
  char arg2;
};

int main(void) {


  p1board = initBoard(p1board);
  p2board = initBoard(p2board);

  int sock_fd;
  int reply_sock_fd;
  int client_count = 0;
  int i;
  /*
  * int yes; This patches a compiler error that prevented compiling
  * with the current compiler settings that complained about it being
  * unused.
  */

  sock_fd = get_server_socket(HOST, HTTPPORT);

  if (start_server(sock_fd, BACKLOG) == -1) {
    printf("start server error\n");
    exit(1);
  }

  while(1) {
    if ((reply_sock_fd = accept_client(sock_fd)) == -1) {
      continue;
    }else{
      if(client_count > 1){
        send_mesg("Gomoku already has two players, sorry! \n", reply_sock_fd);
        close(reply_sock_fd);
        continue;
      }else{
        start_subserver(reply_sock_fd, client_count);
        client_count++;
      }
    }

  }

  for(i = 0; i < HEIGHT; i++){
    free(p1board[i]);
    free(p2board[i]);
  }
  free(p1board);
  free(p2board);
}


//
int get_server_socket(char *hostname, char *port) {
  struct addrinfo hints, *servinfo, *p;
  int status;
  int server_socket;
  int yes = 1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
    printf("getaddrinfo: %s\n", gai_strerror(status));
    exit(1);
  }

  for (p = servinfo; p != NULL; p = p ->ai_next) {
    if ((server_socket = socket(p->ai_family, p->ai_socktype,
      p->ai_protocol)) == -1) {
        printf("socket socket \n");
        continue;
      }
      if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        printf("socket option\n");
        continue;
      }

      if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
        printf("socket bind \n");
        continue;
      }
      break;
    }
    print_ip(servinfo);
    freeaddrinfo(servinfo);
    return server_socket;
  }

  int start_server(int serv_socket, int backlog) {
    int status = 0;
    if ((status = listen(serv_socket, backlog)) == -1) {
      printf("socket listen error\n");
    }
    return status;
  }

  int accept_client(int serv_sock) {
    int reply_sock_fd = -1;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage client_addr;
    char client_printable_addr[INET6_ADDRSTRLEN];

    // 157 is the last line that GDB reports.
    if ((reply_sock_fd = accept(serv_sock,
      (struct sockaddr *)&client_addr, &sin_size)) == -1) {
        perror("socket accept error\n");
      }
      else {
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr),
        client_printable_addr, sizeof client_printable_addr);
        printf("server: connection from %s at port %d\n", client_printable_addr,
        ((struct sockaddr_in*)&client_addr)->sin_port);
      }
      return reply_sock_fd;
    }

void start_subserver(int reply_sock_fd, int client_count) {

  struct arg_s args;
  pthread_t pthread;
  long reply_sock_fd_long = reply_sock_fd;
  args.arg1 = reply_sock_fd_long;

  if(client_count % 2 == 0)
  args.arg2 = 1;
  else if(client_count % 2 == 1)
  args.arg2 = 2;

  if (pthread_create(&pthread, NULL, (void*)subserver, (void*)&args) != 0) {
    printf("failed to start subserver\n");
  }
  else {
    printf("subserver %lu started\n", (unsigned long)pthread);
  }
}

void *subserver(void *arguments) {
  // TODO
  // NEED to implement a "wait" function so that if only one client is connected
  // they don't play the game themselves
  //
  // If someone connects while two clients are already connected
  // need to kill connection and tell them that two players are already playing
  //get the arguments
  struct arg_s *args =  arguments;
  long reply_sock_fd_long = args->arg1;
  char pid = args->arg2;


  int read_count = -1;
  int win;
  int BUFFERSIZE = 256;
  char buffer[BUFFERSIZE+1];

  int reply_sock_fd = (int) reply_sock_fd_long;

  printf("subserver ID = %lu\n", (unsigned long) pthread_self());

  read_count = recv(reply_sock_fd, buffer, BUFFERSIZE, 0);
  buffer[read_count] = '\0';

  printf("%s\n", buffer);

  if((win = gameLoop(reply_sock_fd, pid)) == -1){
    perror("[!!!] error: Game Loop Fail");
  }

  if(win == 1 && pid == 1)
  send_mesg("You Win!", reply_sock_fd);
  else if (win == 2 && pid == 2)
  send_mesg("You Win!", reply_sock_fd);
  else
  send_mesg("You Lose :-(", reply_sock_fd);

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
int gameLoop(int reply_sock_fd, char pid){
 
  int p1win;
  int p2win; 
  //for the first time, white goes first (Player 2)
  gips *player_info;
  gips *other_player;
  int currentTurn;
  //make variables to keep track of the other player

  char other_pid;
  if(pid == 1){
    other_pid = 2;
    pthread_mutex_lock(&play1Moves);
    play1[0] = 3;
    play1[1] = 3;
    pthread_mutex_unlock(&play1Moves);
    p1board[3][3] = 'x';
  }else if(pid == 2) {
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
  if(pid == 1)
    player_info = pack(pid, FALSE, (char)currentTurn, 3, 3, TRUE);
  else
    player_info = pack(pid, FALSE, (char)currentTurn, 3, 3, FALSE);

  //send the first instantiated game board with
  //player1 moved on a center piece
  send_to(player_info, reply_sock_fd);

  int read_count = -1;


  do {
    //receive board of client we are conversing with
    read_count = recv(reply_sock_fd, player_info, sizeof(player_info), 0);
    printf("%d\n", read_count);
    

    //Waiting: if it's not the clients turn, the server will send a gips packet with
    //the clients own PID and waiting set to 1 (true). 
    //while the client is waiting, it should keep itself in a loop, while printing to the user
    //that the other player is making a move
    //once the client receives a packer with waiting set to FALSE then that gips packet
    //will contain the other players moves
    if(player_info->waiting){
      //go and move if it's the clients turn
      if(currentTurn != pid){
        player_info = pack(pid, FALSE, currentTurn, -1, -1, TRUE);
      }
      else{
        int * moves = findOtherMoves(player_info);
        player_info = pack(pid, FALSE, currentTurn, moves[0], moves[1], FALSE);
      }
      send_to(player_info, reply_sock_fd);
    
    
    }else{

      //add the move to the board, and to the respective client arrays keeping track of
      //each players moves
      addMove(player_info->move_a, player_info->move_b, player_info->pid);

      currentTurn = turn(player_info);

      //send OTHER players moves
      //send other PID
      //send  players turn
      //check/sends isWin
      
      if(pid == 1){
        //i hope this is OK because i'm using pass by value, and the actual 'pack' function
        //only modifies copies of values locally
        //pack a gips player with turns of other player, other players pid, current turn,
        //and waiting set to TRUE
        pthread_mutex_lock(&play2Moves);
        other_player = pack(other_pid, FALSE, currentTurn, (char)play2[0], (char)play2[1], TRUE);
        pthread_mutex_unlock(&play2Moves);
      }
      else {
        pthread_mutex_lock(&play1Moves);
        other_player = pack(other_pid, FALSE, currentTurn, (char)play1[0], (char)play1[1], TRUE);
        pthread_mutex_unlock(&play1Moves);
      }

      //switch the turn global var
      pthread_mutex_lock(&whoTurn_access);
      whoTurn = currentTurn;
      pthread_mutex_unlock(&whoTurn_access);
     
      

      //most up-to-date moves are this player 
      if(pid % 2 == 0){
        p2win = check_for_win_server(p2board);

      }else{
        p1win = check_for_win_server(p1board);
      }
      //if it's a win send a packet with other players moves
      //and isWin set to pid of winner
      //return from gameLoop
      if((p1win) || (p2win)){
        other_player->isWin = pid;
        send_to(other_player, reply_sock_fd);
        return other_player->isWin;
      } else {
        send_to(other_player, reply_sock_fd);
      }
    }

  } while(read_count != 0 || read_count != -1);

  return read_count == -1? -1:0; //-1 on fail 0 on success

}

void print_ip( struct addrinfo *ai) {
  struct addrinfo *p;
  void *addr;
  char *ipver;
  char ipstr[INET6_ADDRSTRLEN];
  struct sockaddr_in *ipv4;
  struct sockaddr_in6 *ipv6;
  short port = 0;

  for (p = ai; p !=  NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {
      ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      port = ipv4->sin_port;
      ipver = "IPV4";
    }
    else {
      ipv6= (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      port = ipv4->sin_port;
      ipver = "IPV6";
    }
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("serv ip info: %s - %s @%d\n", ipstr, ipver, ntohs(port));
  }
}

//get the structure of incoming addr
void *get_in_addr(struct sockaddr * sa) {
  if (sa->sa_family == AF_INET) {
    printf("ipv4\n");
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else {
    printf("ipv6\n");
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}

void addMove(char move_a, char move_b, char pid){

  if(pid == 1){
    p1board[(int)move_a][(int)move_b] = 'x';

    pthread_mutex_lock(&play1Moves);
    play1[0] = (int)move_a;
    play1[1] = (int)move_b;
    pthread_mutex_unlock(&play2Moves);
  }
  else if (pid == 2){
    p2board[(int)move_a][(int)move_b] = 'x';

    pthread_mutex_lock(&play2Moves);
    play2[0] = (int)move_a;
    play2[1] = (int)move_b;
    pthread_mutex_unlock(&play2Moves);
  }
  return;
}

//checks if it is this
//make sure turn logic works out
int turn(gips *player_info){

  if(player_info->whoTurn == 1)
  return 2;
  else return 1;
}

char **initBoard(char **board){
  int i;
  //instantiate board
  board = malloc(HEIGHT * sizeof(char *));

  for(i = 0; i < HEIGHT; i++){
    board[i] = malloc(DEPTH);
    memset(&board[i], 0, sizeof(board[i])* strlen(board[i]));
  }

  board[3][3] = 'x';
  return board;
}

int * findOtherMoves(gips *player_info){
  int *moves = malloc(2 * sizeof(int));
  if(player_info->pid == 1){
    pthread_mutex_trylock(&play2Moves);
      moves[0] = play2[0];
      moves[1] = play2[1];
    pthread_mutex_unlock(&play2Moves);
  }else if (player_info->pid == 2){
    pthread_mutex_trylock(&play1Moves);
      moves[0] = play1[0];
      moves[1] = play1[1];
    pthread_mutex_unlock(&play1Moves);
  }

  return moves;
}
