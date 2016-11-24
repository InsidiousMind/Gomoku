#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "gips.h"

//Some functions to compress what we send over the nets

//packs info needed for playing the game into a struct
//using chars to make the package as small as possible

gips * pack(BYTE pid, BYTE isWin, BYTE move_a, BYTE move_b) {
  static gips info;

  info.pid = pid; //1 if player1, 2 if player2
  info.isWin = isWin; //0 if not win, 1 if player 1 win, 2 if player 2 win
  info.move_a = move_a; //move || first board == own player turn,
  info.move_b = move_b; //move         EVERY OTHER board === OTHER PLAYER move
  return &info;
}


//Functions to safely send things over to client, or from client to server
int send_to(gips *info, int sock) {

  int total = 0;
  int bytesleft = sizeof(info), n;
  int len = sizeof(info);
  while (total < len) {
    n = (int) send(sock, &(*(info + total)), bytesleft, 0);
    if (n == -1) {
      perror("[!!!] could not send");
      break;
    }
    total += n; //tally up what was sent
    bytesleft -= n; //bytes left to send
  }
  len = total; // number of bytes actually sent
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

int send_misc(void *thing, int sock){

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
}

//TODO
//Functions to safely receive things from client/server
