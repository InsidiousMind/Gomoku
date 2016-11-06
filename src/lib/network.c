#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "gips.h"
#include "network.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>


//maybe put this in a private functions b/c this function is not being shared
//it is solely for client use


int connect_to_server() {
  // FIXME This is the source for the connection subroutine in the example.
  char *hostname = HOST;
  char *port = HTTPPORT;
  int serverfd;
  struct addrinfo hints, *servinfo, *p;
  int status;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
    printf("getaddrinfo: %s\n", gai_strerror(status));
    return -1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((serverfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      printf("socket socket \n");
      continue;
    }

    if ((status = connect(serverfd, p->ai_addr, p->ai_addrlen)) == -1) {
      close(serverfd);
      perror("socket connect \n");
      continue;
    }
    break;
  }

  freeaddrinfo(servinfo);

  if (status != -1) return serverfd;
  else return -1;
}

int send_to(gips *info, int sock) {
  
  int total = 0;
  int bytesleft = sizeof(info), n;
  int len = sizeof(info);
  while (total < len) {
    n = send(sock, &(*(info + total)), bytesleft, 0);
    if (n == -1) {
      perror("[!!!] could not send");
      break;
    }
    total += n; //tally up what was sent
    bytesleft -= n; //bytes left to send
  }
  len = total; // number of bytes actually sent
  return n == -1 ? -1 : total; //-1 on fail 0 on success
}

int send_mesg(char *str, int sock) {

  int total = 0;
  int bytesleft = strlen(str) * sizeof(char);
  int n;
  int len = bytesleft;

  while (total < len) {
    n = send(sock, str + total, bytesleft, MSG_NOSIGNAL);
    if (n == -1) {
      perror("[!!!] could not send:");
      break;
    }
    total += n;
    bytesleft -= n;
  }
  len = total;
  return n == -1 ? -1 : total;
}

int send_misc(void *thing, int sock){

  int total = 0;
  //watch out for this, this is probably wrong
  int bytesleft = sizeof(*thing);
  int n;
  int len = bytesleft;
  
  while( total < len){
    n = send(sock, thing + total, bytesleft, MSG_NOSIGNAL);
    if (n == -1){
      perror("[!!!] coult not send") ;
      break;
    }

    total += n;
    bytesleft -= n;
  }
  len = total;

  return n == -1 ? -1 : total;
}


int readBytes(int sock, unsigned int x, void *buffer) {
 
  int bytes_read = 0, result;

  while (bytes_read < (signed) x) {

    //can use read here too
    result = recv(sock, buffer + bytes_read, x - bytes_read, 0);
    if (result < 1) {
      perror("[!!!] read error");
    }
    bytes_read += result;
  }
  return bytes_read;
}


