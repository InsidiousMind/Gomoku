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
#include "asgn6-server.h"


int main(void) {
  int client_count = 0;
  server_loop(&client_count);
  if(client_count != 0)
    printf("%s%d\n", "It's not zero!", client_count);
  else
    printf("It's zero.");
}

