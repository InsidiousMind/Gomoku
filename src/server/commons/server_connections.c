#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <string.h>
#include "server_connections.h"

void cnode(c_head **src_node, cList **dest_node);
void to_head(cList **src_node, c_head **dest_node, int size);
bool isSockUp(int sockfd);

void c_add(c_head **head, int sockfd){
 
  cList *newNode = calloc(1, sizeof(cList));
  c_head *t_head = *head;
  newNode->sockfd = sockfd;
  newNode->isPlaying = false;
  
  //this is only the first node case 
  if(*head == NULL) {
    *head = calloc(1, sizeof(c_head));
    to_head(&newNode, head, 1);
    free(newNode);
    return;
  }

  cList *temp = calloc(1, sizeof(cList));
  cnode(head, &temp);
  cList *curr = temp;
  
  while(temp != NULL) {
    temp = temp->next;
    if (curr->next == NULL) {
      curr->next = newNode;
      if(t_head->next == NULL){
        t_head->next = newNode;
        *head = t_head;
      }
      newNode->next = NULL;
      t_head->size++;
      *head = t_head;
      free(temp);
      return;
    }
    curr = temp;
  }
}
void c_update(c_head **head, int sockfd){
  
 cList *temp = calloc(1, sizeof(cList));
 c_head *t_head = *head;
 cnode(head, &temp);
  
 while(temp != NULL) {
  if(t_head->sockfd == sockfd){
    t_head->isPlaying = true;
    *head = t_head;
    return;
  }else if(temp->sockfd == sockfd) {
    temp->isPlaying = true;
    return;
  } else temp = temp->next;
 }
  printf("[!!ERROR!!] - socket never connected or has never been added to connections\n");
}

void c_del(c_head **head, int sockfd){
  
  cList *temp = calloc(1, sizeof(cList));
  cnode(head, &temp);
  cList *curr = temp;
  c_head *t_head = *head; 
  
  if(t_head->size == 1) {
    setPlaying(&t_head, t_head->pairedSockfd);
    t_head = NULL;
    free(t_head);
    *head = t_head;
    return;
  }
  while(temp != NULL){
    if(t_head->sockfd == sockfd){
      setPlaying(&t_head, t_head->pairedSockfd);
      to_head(&(curr->next), head, t_head->size-1);
      curr->next = NULL; 
      free(curr);
      return;
    }
    else if (temp->sockfd == sockfd){

      setPlaying(&t_head, t_head->pairedSockfd);
      //this was the only way, since cannot compare pointers since
      //dealing with two different structs, c_head and cList
      if(t_head->next->sockfd == curr->next->sockfd) 
        t_head->next = temp->next;
      curr->next = temp->next;
      free(temp);

      //update head
      t_head->size--;
      *head = t_head;
      return;
    }

    curr = temp;
    temp = temp->next;
  }
}

//find a not-playing node, that is NOT found
int find(c_head **head, int found){

  int i;
  cList *temp;
  c_head *t_head = *head;
  
  //ignore head, socket in head is only -1
  temp = t_head->next;
  for(i = 1; i < t_head->size; i++) {
    if(temp->isPlaying == false && temp->sockfd != found) {
      return temp->sockfd;
    } else temp = temp->next;
  }
  return -1;
}

void parseConnections(c_head **head) {
  int i;
  c_head *t_head = *head;
  cList *temp;
 
  //ignore head, set temp to socket head is pointing to
  if(t_head->next != NULL) temp = t_head->next;
  else return;

  for (i = 1; i < t_head->size; i++, temp = temp->next) {
    if (!isSockUp(temp->sockfd)) {
      c_del(head, temp->sockfd);
    }
  }
}

bool isSockUp(int sockfd){
  int error = 0;
  socklen_t len = sizeof (error);
  int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
  
  if (retval != 0) {
    /* there was a problem getting the error code */
    fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
    return false;
  }
  if (error != 0) {
    /* socket has a non zero error status */
    fprintf(stderr, "socket error: %s\n", strerror(error));
    return false;
  }
  return true;
}
//sets either playing to false or true, depending on
//if it was set to true or false in the first place
void setPlaying(c_head **head, int sockfd){
  
  int i;
 
  c_head *t_head = *head;
  cList *temp;
  if(t_head->sockfd == sockfd) {
    t_head->isPlaying = !t_head->isPlaying;
    return;
  }
  temp = t_head->next;

  for(i = 1; i < t_head->size; i++, temp = temp->next){
    if(temp->sockfd == sockfd) {
      temp->isPlaying = !temp->isPlaying;
      return;
    }
  }
  printf("No connection with that socket descriptor\n");
}

//copy node
//source, destination
void cnode(c_head **src_node, cList **dest_node){
  c_head *src = *src_node;
  cList *dest = *dest_node;
  
  dest->sockfd = src->sockfd;
  dest->isPlaying = src->isPlaying;
  dest->next = src->next;
  dest->pairedSockfd = src->pairedSockfd;
}

void to_head(cList **src_node, c_head **dest_node, int size){
  c_head *dest = *dest_node;
  cList *src = *src_node;

  dest->sockfd = src->sockfd;
  dest->isPlaying = src->isPlaying;
  dest->size = size;
  dest->next = src->next;
  dest->pairedSockfd = src->pairedSockfd;
}

int *getSockets(c_head **t_head){
  int i;
  c_head  *head = *t_head;
  cList *temp;
  
  int *sockets = calloc(head->size, sizeof(int));
  
  sockets[0] = head->sockfd;

  temp = head->next;

  for (i = 1; i < head->size; i++, temp = temp->next) {
    sockets[i] = temp->sockfd;
  }
  return sockets;
}
