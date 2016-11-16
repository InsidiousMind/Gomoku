#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include "../../lib/database.h"
#include "../../lib/usermgmt.h"
#include "../../lib/gips.h"

void recPlayer(pthread_mutex_t **temp_head_access, int uPID, int fd, int isWin, Node *head, char *username, char PID, int sockfd)
{
  
  pthread_mutex_t head_access = **((pthread_mutex_t **)temp_head_access);

  pthread_mutex_lock(&head_access);
  
  Player *player; 

  if((player = query(username, uPID, fd, head, FALSE)) == NULL){
    player = calloc(1, sizeof(Player));
    player->userid = uPID;
    player->username = username;
    if(PID == isWin){
      player->wins = 1;
      player->losses=0;
    }
    else{
      player->wins = 0;
      player->losses = 1;
    }
    insert(uPID, fd, player, &head)  ;
    printf("player commited to DB");
    //send stats
    //
  } else {

    if(PID == isWin){
      player->wins += 1; 
    }else{
      player->losses+=1; 
    }
    update(uPID, fd, player, head);
  }
  
  pthread_mutex_unlock(&head_access);
  
  send(sockfd, player, sizeof(Player), 0);

}


