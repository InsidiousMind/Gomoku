/* Program: Programming Assignment 7, Networked Gomoku Game
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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "../lib/database.h"
#include "commons/asgn6-server.h"


int main(int argc, char *argv[]) {

  int fd = 0, index = 0;

  //init mutex for access to the database
  pthread_mutex_t head_access = PTHREAD_MUTEX_INITIALIZER;

  Node *game_head = malloc(sizeof(Node));
  if(argc <= 1) {
    fprintf(stderr, "Usage: './server filename\n'");
    exit(1);
  }else{
    printf("Load data from previous file (if it exists)? [Y/n] ");
    char c = getchar();

    if(c == 'y' || c == 'Y'){

      fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
      persist(fd, &index, &game_head, argv[1]);

    } else fd = open(argv[1], O_TRUNC|O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  }
  serverLoop(fd, &game_head, &head_access);
  free_gameList(&game_head);
  close(fd);
  return 0;
}
