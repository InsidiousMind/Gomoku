#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "database.h"
#include "network.h"
#include "gips.h"

int login(int sock, int uniquePID, char *username) {

  int upid = uniquePID;
  //get pid from the server based on the username.
  send(sock, &upid, sizeof(int), 0);
  send_mesg(username, sock);
  recv(sock, &upid, sizeof(int), 0);
  return upid;
}

int check_valid_ptr(void *ptr) {
  // This is to pacify cppcheck.
  if (ptr == NULL) {
    return 0;
  } else {
    return 1;
  }
}
