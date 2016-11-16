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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../lib/database.h"
#include "commons/asgn6-server.h"
#include "commons/server_db.h"

void die(const char *message);

int main(int argc, char *argv[]) {

  int index = 0, fd = 0;
  char *filename;
  Node *head = NULL;
  if(argc <= 1){
    fprintf(stderr, "Usage: './server filename\n'");
    exit(1);
  }else{
    //y/n, if it's not y or Y then it's assumed to be no
    printf("Load data from previous file (if it exists)? [Y/n] ") ;
    char c = getchar();
   if(c == 'y' || c == 'Y') {
    filename = malloc(strlen(argv[1]) + 1);
    memset(filename, 0, sizeof(*filename));
    fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
   }else fd = open(argv[1], O_TRUNC|O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  }
  if(fd < 0) die("[ERROR] open failed");

  serverLoop(fd, &head);

  free(filename);
  close(fd);
}

void die(const char *message){
  if(errno)
    perror(message);
  else
    printf("ERROR: %s\n", message);
  exit(1);
}
