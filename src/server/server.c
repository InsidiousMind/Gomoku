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
// Why is this commented out?
//#include "../lib/glogic.h"

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


//shouldn't need a mutext lock for these because they are only accessed by their respective threads

char **p1board;
char **p2board;
//just for storing args which are going to be passed to pthread_create

pthread_mutex_t play1Moves = PTHREAD_MUTEX_INITIALIZER;
int play1[2];
pthread_mutex_t play2Moves = PTHREAD_MUTEX_INITIALIZER;
int play2[2];

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
  		start_subserver(reply_sock_fd, client_count);
      client_count++; 
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

	if ((reply_sock_fd = accept(serv_sock, 
					(struct sockaddr *)&client_addr, &sin_size)) == -1) {
		printf("socket accept error\n");
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
 
  if(client_count == 0)
    args.arg2 = 1;
  else if(client_count == 1)
    args.arg2 = 2;
  else
    return;
  //just want to return, our game can only accomodate 2 players

	if (pthread_create(&pthread, NULL, (void*)subserver, (void*)&args) != 0) {
		printf("failed to start subserver\n");
	}
	else {
		printf("subserver %ld started\n", (unsigned long)pthread);
	}
}

void *subserver(void *arguments) {
  
  //get the arguments 
  struct arg_s *args =  arguments;
  long reply_sock_fd_long = args->arg1;
  char pid = args->arg2;

 
  int read_count = -1; 
  int win;
  int BUFFERSIZE = 256;
  char buffer[BUFFERSIZE+1];

  int reply_sock_fd = (int) reply_sock_fd_long;
 
  printf("subserver ID = %ld\n", (unsigned long) pthread_self());
  
  read_count = recv(reply_sock_fd, buffer, BUFFERSIZE, 0);
  buffer[read_count] = '\0';
  printf("%s\n", buffer);

  
  if((win = gameLoop(reply_sock_fd, pid)) == -1){
    perror("[!!!] error: Game Loop Fail");
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
int gameLoop(int reply_sock_fd, char pid){
  //for the first time, white goes first (Player 2)
  gips *player_info;
  gips *other_player;
  char isTurnc = 2;

  char is_otherTurnc; 
  char other_pid;
  if(pid == 1) other_pid = 2;
  if(pid == 2) other_pid = 1;
  
    
  player_info = pack(pid, 0, isTurnc, 3, 3);

//  int lastTurn = 2;

  //send an instantiated GIPS board
  send_to(player_info, reply_sock_fd);

  int read_count = -1;

   
  while(read_count != 0 || read_count != -1){
    read_count = recv(reply_sock_fd, player_info, sizeof(player_info), 0);

    addMove(player_info->move_a, player_info->move_b, player_info->pid);

    player_info->isTurn = turn(player_info);
    
    /*modify last turn as needed
    if(player_info->isTurn == 1) lastTurn = player_info->pid;
    else if (player_info->isTurn == 0 && pid == 1) lastTurn = 2;
    else lastTurn = 1; */

    if(player_info->isTurn) is_otherTurnc = 0;
    else is_otherTurnc = 1;
    //send other players moves
    if(pid == 1){
      pthread_mutex_lock(&play2Moves);
      other_player = pack(other_pid, 0, is_otherTurnc, (char)play2[0], (char)play2[1]);
      pthread_mutex_unlock(&play2Moves);
    }
    else { 
      pthread_mutex_lock(&play1Moves);
      other_player = pack(other_pid, 0, is_otherTurnc, (char)play1[0], (char)play1[1]);
      pthread_mutex_lock(&play1Moves);
    }
    send_to(other_player, reply_sock_fd);

    //check_for_win_server(&player_info, board);
    //how to send back to client 
    if(player_info->isWin != 0) 
      return player_info->isWin;
    //update gameserver with moves of other player, send updated GIPS back
  }

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


int turn(gips *info){
  if((info->isTurn == 1) && (info->pid == 1))       return 2;
  else if ((info->isTurn = 1) && (info->pid == 2))  return 1;
  else if ((info->isTurn = 0) && (info-> pid == 1)) return 1;
  else if ((info -> isTurn = 0) && (info->pid == 2))   return 2;

  return 0;
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
