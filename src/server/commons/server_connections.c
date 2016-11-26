#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "server_connections.h"

void cnode(c_head **src_node, cList **dest_node);
void to_head(cList **src_node, c_head **dest_node, int size);

void add(c_head **head, int sockfd){
 
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
void update(c_head **head, int sockfd){
  
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

void del(c_head **head, int sockfd){
  
  cList *temp = calloc(1, sizeof(cList));
  cnode(head, &temp);
  cList *curr = temp;
  c_head *t_head = *head; 
  
  if(t_head->size == 1) {
    t_head = NULL; 
    free(t_head);
    *head = t_head;
    return;
  }
  while(temp != NULL){
    if(t_head->sockfd == sockfd){
      to_head(&(curr->next), head, t_head->size-1);
      curr->next = NULL; 
      free(curr);
      return;
    }
    else if (temp->sockfd == sockfd){

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

//copy node
//source, destination
void cnode(c_head **src_node, cList **dest_node){
  c_head *src = *src_node;
  cList *dest = *dest_node;
  
  dest->sockfd = src->sockfd;
  dest->isPlaying = src->isPlaying;
  dest->next = src->next;
}

void to_head(cList **src_node, c_head **dest_node, int size){
  c_head *dest = *dest_node;
  cList *src = *src_node;

  dest->sockfd = src->sockfd;
  dest->isPlaying = src->isPlaying;
  dest->size = size;
  dest->next = src->next;
}
