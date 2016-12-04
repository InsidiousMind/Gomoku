#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
#include "asgn6-server.h"
#include "game_thread.h"

//Shared libraries
#include "../../lib/gips.h"
#include "../../lib/database.h"
#include "server_connections.h"

void *get_in_addr(struct sockaddr *sa); //get info of incoming addr in struct
void print_ip(struct addrinfo *ai); //prints IP
int get_server_socket(char *hostname, char *port); //get a socket and bind to it
int start_server(int serv_socket, int backlog);  //starts listening on port for inc connections
int accept_client(int serv_sock); //accepts incoming connection
int* startGame(c_head **head);


void serverLoop(int fd, Node **temp, pthread_mutex_t *head_access){
  
  c_head *c_head = NULL;
  int sock_fd;
  Node *game_head = *((Node **) temp);

  gameArgs *gameSrvInfo = malloc(sizeof(gameArgs));

  // game info for managing the player records. Once the game ends, it is automatically written
  // to the file
  gameSrvInfo->fd = fd;
  gameSrvInfo->head = game_head;
  gameSrvInfo->head_access = head_access;
  gameSrvInfo->c_head = c_head;
  pthread_t pthread;

  //make the thread detached
  //pthread_attr_t attr;
  //pthread_attr_init(&attr);
  //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


  sock_fd = get_server_socket(HOST, HTTPPORT);
  if (start_server(sock_fd, BACKLOG) == -1){
    perror("[!!!] error on server start");
    exit(1);
  }
  
  /*once two clients connect init a game server
   *              Game Server
   *              /        \
   *             /          \
   *   Client Thread      Client Thread  ( both attached to game server)
   *
   *   this gives more control over client threads
   *   For example, now we can use pthread_join in Game server
   *   to wait for each client thread to finish
   *  reducing memory leaks
   */

  int r_sockfd;
  int *start_socks;
 //TODO: Has to be a way to inform client that it's paired client has disconnected
  //
while(true){
 
    if ((r_sockfd = accept_client(sock_fd)) == -1)
      continue;
    c_add(&c_head, r_sockfd);

    parseConnections(&c_head);
    
    if( (start_socks = startGame(&c_head)) != NULL) {
      gameSrvInfo->reply_sock_fd[0] = start_socks[0];
      gameSrvInfo->reply_sock_fd[1] = start_socks[1];
      if((pthread_create(&pthread, NULL, (void*) startGameServer, (void*) gameSrvInfo)) != 0)
       printf("Failed to start Game Server");
      
      //invert isPlaying
      setPlaying(&c_head, start_socks[0]);
      setPlaying(&c_head, start_socks[1]);
      
      free(start_socks);
    }
  }
}

//checks for a valid game, and if it can find one
//returns the two socket connections to start one
int* startGame(c_head **head){
  
  int *start_socks = calloc(2, sizeof(int));
 
  if( (start_socks[0] = find(head, -1)) == -1)
    return NULL;
  if( (start_socks[1] = find(head, start_socks[0])) == -1)
    return NULL;
  return start_socks;
}

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

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((server_socket = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      printf("socket socket \n");
      continue;
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      printf("socket option\n");
      continue;
    }
    if (fcntl(server_socket, F_SETFD, O_NONBLOCK)  == -1){
      printf("[!!ERROR!!] socket option: fcntl \n");
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
          (struct sockaddr *) &client_addr, &sin_size)) == -1) {
    perror("socket accept error\n");
  } else {
    inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr),
        client_printable_addr, sizeof client_printable_addr);
    printf("server: connection from %s at port %d\n", client_printable_addr,
        ((struct sockaddr_in *) &client_addr)->sin_port);
  }
  return reply_sock_fd;
}

void print_ip(struct addrinfo *ai) {
  struct addrinfo *p;
  void *addr;
  char *ipver;
  char ipstr[INET6_ADDRSTRLEN];
  struct sockaddr_in *ipv4;
  struct sockaddr_in6 *ipv6;
  short port = 0;

  for (p = ai; p != NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {

      ipv4 = (struct sockaddr_in *) p->ai_addr;
      addr = &(ipv4->sin_addr);
      port = ipv4->sin_port;
      ipver = "IPV4";


    } else {
      ipv6 = (struct sockaddr_in6 *) p->ai_addr;
      addr = &(ipv6->sin6_addr);
      port = ipv4->sin_port;
      ipver = "IPV6";
    }

    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("serv ip info: %s - %s @%d\n", ipstr, ipver, ntohs(port));
  }
}

//get the structure of incoming addr
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    printf("ipv4\n");

    return &(((struct sockaddr_in *) sa)->sin_addr);

  } else{

    printf("ipv6\n");
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
  }

}
