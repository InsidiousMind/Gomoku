#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gips.h"
//Some functions to compress what we send over the nets

//packs info needed for playing the game into a struct
//using chars to make the package as small as possible
#ifndef GIPS_SIZE
#define GIPS_SIZE 5
#endif

gips * pack(BYTE pid, BYTE isWin, BYTE move_a, BYTE move_b, BYTE isEarlyExit) {
  static gips info;
  info.pid = pid; //1 if player1, 2 if player2
  info.isWin = isWin; //0 if not win, 1 if player 1 win, 2 if player 2 win
  info.move_a = move_a; //move || first board == own player turn,
  info.move_b = move_b; //move         EVERY OTHER board === OTHER PLAYER move
  info.isEarlyExit = isEarlyExit;
  return &info;
}


//Functions to safely send things over to client, or from client to server
int send_to(gips *info, int sock) {

  int total = 0;
  ssize_t n;
  char *gipsArr = calloc(5, sizeof(char));
  gipsArr[0] = info->pid;
  gipsArr[1] = info->isWin;
  gipsArr[2] = info->move_a;
  gipsArr[3] = info->move_b;
  gipsArr[4] = info->isEarlyExit;
  
  size_t bytesleft = GIPS_SIZE;
  int len = GIPS_SIZE;
  while (total < len) {
    n = send(sock, &(*(gipsArr+total)), sizeof(char), MSG_NOSIGNAL);
    if (n == -1) {
      perror("[!!!] could not send");
      break;
    }
    total += n; //tally up what was sent
    bytesleft -= n; //bytes left to send
  }
  len = total; // number of bytes actually sent
  free(gipsArr);
  return n == -1 ? -1 : total; //-1 on fail 0 on success
}

int receive_gips(int sock, gips **info){
  gips *tempInfo = *info;
  char gipsArr [5];
  int bytes_to_receive = GIPS_SIZE;
  int len = GIPS_SIZE, total = 0;
  char testBuf;
  ssize_t n;
  while(true) {
    n = recv(sock, &testBuf, sizeof(char), MSG_PEEK | MSG_DONTWAIT);
    if(n == -1 || n == 0){
      perror("[!!!] could not recv/clientdc in receive_gips");
      break;
    }
    if(testBuf == '\v'){
      /*do nothing, let poll_for_chat do it's thang*/
    }else {
      while (total < len) {
        n = recv(sock, &gipsArr[total], sizeof(char), 0);
        if (n == -1 || n == 0) {
          perror("[!!!] could not recv/or clientDC in receive_gips");
          break;
        }
      }
      total += n;
      bytes_to_receive -= n;
      break;
    }
  }
  len = total;
  
  tempInfo->pid = gipsArr[0];
  tempInfo->isWin = gipsArr[1];
  tempInfo->move_a = gipsArr[2];
  tempInfo->move_b = gipsArr[3];
  tempInfo->isEarlyExit = gipsArr[4];
  *info = tempInfo;
  
  return n == -1 ? -1 : total; //-1 on fail 0 on success
}

int send_mesg(char *str, int sock) {

  int total = 0;
  int bytesleft = (int) (strlen(str) * sizeof(char));
  int n;
  int len = bytesleft;

  while (total < len) {
    n = (int) send(sock, str + total, bytesleft, MSG_NOSIGNAL);
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

/*int send_misc(void *thing, int sock){

  int total = 0;
  //watch out for this, this is probably wrong
  int bytesleft = sizeof(*thing);
  int n;
  int len = bytesleft;

  while( total < len){
    n = (int) send(sock, thing + total, bytesleft, MSG_NOSIGNAL);
    if (n == -1){
      perror("[!!!] could not send") ;
      break;
    }

    total += n;
    bytesleft -= n;
  }
  len = total;

  return n == -1 ? -1 : total;
}*/

//TODO
//Functions to safely receive things from client/server
