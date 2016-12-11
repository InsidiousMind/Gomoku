/* Program: Programming Assignment 5, create a shell
* Author: Andrew Plaza
* Github: https://github.com/InsidiousMind/Gomoku
* Date: October 31 2016
* File Name: asgn6-server.c
* compile: make server
* run ./server
* debug: gdb ./server
*
* A server program that communicates to two (and only two) client programs
* in order to play the game Gomoku.
* As per the rules of Gomoku, player one (black) moves automatically
* to the middle, and then the first player to connect 5 wins
* This program uses multithreading (one client per thread)
* In the future, this program should make use of more structs instead of
* global vars
* this program uses game_thread.h private functions
* libraries to run. They are linked when running make server
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "../lib/andrews-db-prog.h"
///#include "../lib/database.h"
#include "commons/asgn6-server.h"
#include "commons/server_db.h"

void die(const char *message);

int main(int argc, char *argv[]) {

  int fd = 0, index = 0;
  char *filename;

  //init mutex for access to the database
  pthread_mutex_t head_access = PTHREAD_MUTEX_INITIALIZER;

  Node *head = malloc(sizeof(Node));
  if(argc <= 1){
    fprintf(stderr, "Usage: './server filename\n'");
    exit(1);
  }else{
    printf("Load data from previous file (if it exists)? [Y/n] ");
    char c = getchar();

    if(c == 'y' || c == 'Y'){
      filename = calloc(1, strlen(argv[1]) + 1);
      strcpy(filename, argv[1]);
      fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
      //persist(fd, &head);
      persist(fd, &index, &head, filename);
    } else fd = open(argv[1], O_TRUNC|O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  }

  serverLoop(fd, &head, &head_access);

  free(filename);
  close(fd);
}


