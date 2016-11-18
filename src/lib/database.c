/*
 * Author: Sean Batzel and Andrew Plaza
 * Player database consisting of an ID number, last name, first name, number of wins, number of losses, and number of ties.
 * Modified to add commands to modify the list during runtime.
 * Modified to add file persistence to the database.
 * Modified to remove the "delete" command.
 * Compile: cc asgn1-batzels4.c -o asgn4
 * Input:
 * 	Add a new player:	+ userid lastname firstname wins losses ties
 *	Update existing:	* userid wins losses ties
 *	Find a player:		? userid
 *	Stop the program:	# <prints all entries>
 * Note:
 * 	If a userid already exists and is added to the database, output will be "ERROR - userid exists."
 *	If a userid does not exist and is queried, or updated, output will be "ERROR - player does not exist."
 */

// Printf calls will be used on the server for debugging.
// Scanf calls should be replaced.

#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include "gips.h"


//the ID of insert must be unique
int insert(int id, int fd, Player *player, Node **head) {


  print_player(player);
  
  Node *tmp = *head;


  //make sure the ID we are inserting is unique
  if (tmp != NULL) {
    if (tmp->player_id == id) {
      printf("Player ID already exists.\n");
      free(tmp);
      return FALSE;
    }
    while (tmp != NULL) {
      if (tmp->player_id == id) {
        printf("Player ID already exists.\n");
        free(tmp);
        return FALSE;
      } else {
        tmp = tmp->next;
      }
    }
  }

  free(tmp);


  Node *x = calloc(1, sizeof(Node));
  x->player_id = id;
  int offset = lseek(fd, 0, SEEK_END);
  x->index = offset;
  
  if ((*head) != NULL && (*head)->player_id == id) {
    printf("ERROR - Player ID already exists.\n");
    free(x);
    return FALSE;
  }
 
  else if (*head == NULL || (*head)->player_id > x->player_id){
    x->next = *head;
    *head = x;
    write(fd, player, sizeof(Player));

  } else {

    tmp = *head;

    while (tmp->next !=NULL && tmp->next->player_id <= x->player_id) {

      if (x->player_id == tmp->next->player_id) {
        printf("ERROR - Player ID already exists.\n");
        free(x);
        return 0;
      }

      tmp =  tmp->next;
    }

    x->next = tmp->next;
    tmp->next = x;
    write(fd, player, sizeof(Player));

  }

  return TRUE;

}

int update(int id, int fd, Player *player, Node *head) {
  Node *tmp = head;
  
  
  while (tmp->next != NULL && tmp->player_id != id)
    tmp = tmp->next;


  if (tmp->player_id == id) {
 
    Player *tmp2 = (Player *)malloc(sizeof(Player));
    lseek(fd, tmp->index, SEEK_SET);
    read(fd, tmp2, sizeof(Player));
    
    print_player(tmp2);
   
    tmp2->wins = player->wins;
    tmp2->losses = player->losses;
    tmp2->ties = player->ties;
  
  
    print_player(tmp2);
  
    lseek(fd, tmp->index, SEEK_SET);
    lseek(fd, tmp->index, SEEK_SET);

    write(fd, tmp2, sizeof(Player));
    free(tmp2);

  } else {

    if (tmp->next == NULL) {

      printf("ERROR - Player does not exist.\n");
      return FALSE;

    }
  }

  return TRUE;
}

void print_list(Node *head) {
  printf("The linked list: ");
  Node *tmp = head;
  while (tmp != NULL){
    printf("(%d, %d), ", tmp->player_id, tmp->index);
    tmp = tmp->next;
  }
  printf("\n");
}

void print_file(int fd) {
  printf("Players:\n");
  lseek(fd, 0, SEEK_SET);
  Player *p = (Player *)malloc(sizeof(Player));
  while (read(fd, p, sizeof(Player)) != 0){
    printf("%d,%s, %d, %d, %d\n", p->userid, p->username, p->wins, p->losses, p->ties);
  }
  free(p);
  printf("End\n");
}

void print_player(Player *player){

  printf("%d, %s, %d, %d, %d\n", player->userid,
      player->username,
      player->wins,
      player->losses,
      player->ties);
}

Player *create_player(int pid, char *username) {
  
  Player *x = calloc(1, sizeof(Player));
  x->userid = pid;
  x->username = username;
  x->wins = 0;
  x->losses = 0;
  x->ties = 0;
  return x;
}

Player *create_player_up(int pid, int wins, int losses, int ties) {
  Player *x = (Player *)malloc(sizeof(Player));
  x->userid = pid;
  x->wins = wins;
  x->losses = losses;
  x->ties = ties;
  return x;
}


//check if username/id combo already exists
//return FALSE if it does exist, TRUE if it does not
Player *query(char *username, int id, int fd, Node *head, int verbose) {

  Node *tmp = head;
  while (tmp != NULL){
    if (tmp->player_id == id){
      Player *tmp2 = calloc(1, sizeof(Player));
      lseek(fd, tmp->index, SEEK_SET);
      read(fd, tmp2, sizeof(Player));
      if(verbose == TRUE)
        printf("QUERY: %d,%s, %d, %d, %d\n", tmp2->userid,
          tmp2->username, tmp2->wins, tmp2->losses, tmp2->ties);
      if (tmp2->username == username){
        return tmp2;
      } else {
        if(verbose == TRUE)
          printf("That id number belongs to a different username.");
        return NULL;
      }
    } else {
      tmp = tmp->next;
    }
  }
  if(verbose == TRUE)
    printf("Player with id %d not found.", id);
  return NULL;
}

//void addPlayer(int id, )


int readp(int fd, int index, Player *play){
  lseek(fd, index*sizeof(Player),0);
  return read(fd, play, sizeof(Player));
}

Node **persist(int fd, Node **head){
  
  //Node *tmp = *head;

  //Player *tmp2 = malloc(sizeof(Player));
  int index = 0;
  Node *tmp = *head;
  
  Player *tmp2 = calloc(1, sizeof(Player));

  while(readp(fd, index, tmp2) != -1){
    Player *newp = calloc(1, sizeof(Player));
    newp = tmp2;  
    insert(newp->userid, fd, newp, &tmp);
    index++;
  }

  *head = tmp;

  free(tmp2);
  return head;
}



void printp(int fd, int index){
  Player play;
  readp(fd,index,&play);
  printf("BEFORE: %d, %s, %d, %d, %d\n", 
                                  play.userid, 
                                  play.username,
                                  play.wins, 
                                  play.losses, 
                                  play.ties);
  return;
}
