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

Player *server_add_user(char *username, int id, int fd, Node *head) {
  // Check if the username exists in the database.
  // If it does, tell the client what pid their name is given.
  // Otherwise, create a new database entry and return THAT pid.
  Player *x;
  printf("Looking for player: %d %s", id, username);
  x = get_player_by_name(username, id, fd, head, FALSE);
  if (check_valid_ptr(x)) {
    printf("Player found, returning.");
  } else if (!check_valid_ptr(x)){
    printf("That player doesn't exist yet.");
    x->userid = id;
    x->last = username;
    x->wins = 0;
    x->losses = 0;
    x->ties = 0;
    insert(id, fd, x, &head);
    printf("Player added.");
  }
  return x;
}
