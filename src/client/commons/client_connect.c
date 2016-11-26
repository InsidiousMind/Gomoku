#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "../../lib/gips.h"


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

int login(int sock, int uniquePID, char *username) {

  int upid = uniquePID;
  //get pid from the server based on the username.
  send(sock, &upid, sizeof(int), 0);
  send_mesg(username, sock);
  recv(sock, &upid, sizeof(int), 0);
  return upid;
}


