#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>

#include "../../lib/database.h"
#include "../../lib/gips.h"

//record player data to player's entry in struct
//if player doesn't exist, create an entry
void recPlayer(pthread_mutex_t **temp_head_access, int uPID, int fd, 
    int isWin, Node *head, char *username, char PID, int sockfd)
{
  
  pthread_mutex_t head_access = **((pthread_mutex_t **)temp_head_access);

  pthread_mutex_lock(&head_access);
  
  Player *player; 
  
  if(doesPlayerExist(&head, uPID, username) == FALSE){
    
    player = calloc(1, sizeof(Player));
   
    strncpy(player->username, username, 20);
    player->userid = uPID;
  
    if(PID == isWin){
      head = add(fd, getIndex(fd), &head, uPID, 1, 0, 0, username);
    }else{
      head = add(fd, getIndex(fd), &head, uPID, 0, 1, 0, username);
    }
  }else{
    if(PID == isWin)
      player = update(fd, &head, uPID, 1, 0, 0);
    else
      player = update(fd, &head, uPID, 0, 1, 0);
  }


  player = getPlayer(uPID, fd, username, &head);
  send(sockfd, player, sizeof(Player), 0);

  pthread_mutex_unlock(&head_access);

}

