#include "database.h"
#include "network.h"
#include "gips.h"

int login(int sock, char *username) {
  //get pid from the server based on the username.
  gips *z = get_server_login(sock, username); // TODO Implement this.
  return z->pid;
}

int server_add_user(int sock, char *username) {
  // Check if the username exists in the database.
  // If it does, tell the client what pid their name is given.
  // Otherwise, create a new database entry and return THAT pid.
  return 0; // This just stops clang from whining.
}
