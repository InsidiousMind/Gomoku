#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void INThandle(int sig){

  char c;

  signal(sig, SIG_IGN);

  printf("Do you REALLY want to quit? [y/n]");

  c = getchar();

  if(c == 'y' || c == 'Y')
    exit(0);
  else
    signal(SIGINT, INThandle);
  //grab the newline char so it doesn't screw stuff up
  getchar();
}

