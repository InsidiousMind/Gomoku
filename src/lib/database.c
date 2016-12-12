/* Program: Programming Assignment 4, Binary Files
 * author: Andrew Plaza
 * Date: Sept 29, 2016
 * File Name: asgn4-plazaa2.c
 * compile: cc -o asgn4.out asgn4-plazaa2.c -g -Wall
 * run: ./asgn4.out
 * debug: gdb ./asgn4.out
 *
 * This C program accepts player records from the keyboard.
 * '+ userid lastname firstname wins losses ties' to add
 * '* userid wins losses ties' to update
 * '? userid' to query
 * '#' to terminate program
 * Records are read and then written in binary form to a file.
 * There is an option to load this file the next time the program is run.
 * The program keeps data relating to where each record is in a file with a linked list (struct node).
 * Upon termination the records and linked list are printed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "database.h"

//method definitions (same order as methods in file)

void readp(int fd, int index, Player *play);
void writep(int fd, int index, Player *play);

//read in previous records

void query(int fd, Node **head);

void free_gameList(Node **head);
void die(const char *message);

//LL = Linked List

//SAMPLE MAIN (DO NOT DELETE)

void readp(int fd, int index, Player *play){
  lseek(fd, index*sizeof(Player),0);
  if(read(fd, play, sizeof(Player)) == -1)
    die("[ERROR] read failed");
}

void writep(int fd, int index, Player *play){
  struct player rec = *play;
  lseek(fd, index*sizeof(Player), 0);
  if(write(fd, &rec, sizeof(Player)) == -1)
    die("[ERROR] write failed");
}

void persist(int fd, int *index, Node **head, char *filename){
  int size, i;
  int temp_i = *index;

  Node *temp = *head;
  struct stat st;
  struct player rec;

  //stat file size, convert to index
  stat(filename, &st);
  size = st.st_size/sizeof(Player);
  if (size == 0) return;

  for(i = 0; i < (size); i++){
    Node *newn = calloc(1, sizeof(Node));

    readp(fd, temp_i, &rec);
    newn->userid = rec.userid;
    newn->index = temp_i;
    insert(&temp, newn);
    temp_i++;
  }
  *index = temp_i;
  *head = temp;
}


//print functions
void print_nodes(Node **head){
  Node *node = *head;
  printf("The linked list: ");
  while(node != NULL){
    printf("(%d, %d)", node->userid, node->index);
    node = node->next;
  }
  printf("\n");
}

void print_players(int fd, Node **head){
  Node *temp = *head;
  while(temp != NULL){
    printp(fd, temp->index);
    temp = temp->next;
  }
}

//takes file and index of player
//reads with readp into player struct
//prints it
void printp(int fd, int index){
  struct player play;
  readp(fd, index, &play);

  printf("%d, %s, %d, %d, %d \n", play.userid,
      play.username,
      play.wins,
      play.losses,
      play.ties);
  return;

}

//creates a new node (calloc) and writes it to binary file
//inserts it into struct


//inserts node into LL
void insert(Node **head, Node *newNode){
  Node *temp = *head;

  if(temp == NULL){
    *head = newNode;
    return;
  }

  if(newNode->userid < temp->userid) {
    //if need to move head back must do it outside loop
    newNode->next = *head;
    *head = newNode;
    return;
  }

  //else we can just insert it into the right pos in the list
  Node *curr;
  curr = temp;

  while(temp->next !=NULL) {
    temp = temp -> next;

    if(newNode->userid < temp->userid) {
      curr->next = newNode;
      newNode->next=temp;
      return;
    }
    //if we are at the end of the list
    curr = temp;
  }

  if(curr->next == NULL){
    curr->next = newNode;
    newNode->next = NULL;
    return;
  }
}

void update(int fd, Node **head, int userid, int wins, int losses, int ties){

  Node *temp = *head;
  //declare struct, don't need to use memory on the heap
  Player *prec = calloc(1, sizeof(Player));


  //find data for playerid user entered, change info
  while(temp != NULL){
    if(temp->userid == userid) {

      printf("%s", "BEFORE: ");
      printp(fd, temp->index);

      readp(fd, temp->index, prec);

      prec->userid = userid;
      prec->wins += wins;
      prec->losses += losses;
      prec->ties += ties;

      writep(fd, temp->index, prec);

      printf("%s", "AFTER: ");
      printp(fd, temp->index);
      return;
    }else temp = temp->next;
  }
  printf("[!!ERROR!!] - player does not exist.\n");
}

void query(int fd, Node **head){
  int userid;
  Node *temp = *head;
  scanf("%d", &userid);

  //find data user needs based on given ID
  //print that player data
  while(temp != NULL){
    if(temp->userid == userid) {
      printf("QUERY: ");
      printp(fd, temp->index);
      //make input look nicer
      printf("> ");

      return;
    }else{
      temp = temp->next;
    }
  }
  //ERROR
  printf("%s\n", "ERROR - player does not exist.");

  //make input look nicer
  printf("> ");

  return;
}

//free memory starting from head
void free_gameList(Node **head){
  Node *node = *head;
  Node *temp;
  while(node != NULL) {
    temp = node;
    node = node->next;
    temp->next = NULL;
    free(temp);
  }
  *head = NULL;
}

void die(const char *message){
  if(errno)
    perror(message);
  else
    printf("ERROR: %s\n", message);
  exit(1)
      ;
}

Node *add(int fd, int index, Node **head, Player **play) {

  Player *prec = *((Player**) play);
  Node *temp = *head;
  while(temp != NULL){
    if(temp->userid == prec->userid){
      printf("ERROR - userid exists. Did you mean to update?\n");
      printp(fd, temp->index);
      return *head;
    } else temp = temp -> next;
  }

  //reset temp after iterating through LL
  temp = *head;
  prec->index = index;
  //write node to binary file
  writep(fd, index, prec);

  printf("%s", "ADD: ");
  printp(fd, prec->index);

  Node *newNode = calloc(1, sizeof(Node));

  newNode->userid = prec->userid;
  newNode->index = prec->index;

  insert(&temp, newNode);
  *head = temp;
  //make input look nicer

  return *head;
}


bool doesPlayerExist(Node **head, int uPID, char *username){
  Node *temp = *head;
  char t_username[21];
  strncpy(t_username, username, 20);

  //find data user needs based on given ID
  //print that player data
  while(temp != NULL){
    if(temp->userid == uPID) {
      return true;
    }else{
      temp = temp->next;
    }
  }
  return false;
}

bool isPlayerTaken(Node **head, int uPID, char *username, int fd){
  Node *temp = *head;
  char t_username[21];
  strncpy(t_username, username, 20);

  //find data user needs based on given ID
  //print that player data
  while(temp != NULL){
    if(temp->userid == uPID) {
    Player play;

    readp(fd, temp->index, &play);
    if(strncmp(play.username, username, 20) != 0)
      return true;
    else if(strncmp(play.username, username, 20) == 0)
      return false;
    }else temp = temp->next;
  }
  return false;
}

int getIndex(int fd){
  int index=0;
  struct player rec;
  int rd = readnp(fd, index, &rec);
  if(rd == 0 || rd == -1) return index;

  while (true){
    if(rd == 0 || rd == -1) break;
    index++;
    rd = readnp(fd, index, &rec);
  }
  return index;
}

int readnp(int fd, int index, Player *play){
  lseek(fd, index*sizeof(Player), 0);
  return read(fd, play, sizeof(Player));
}

Player* getPlayer(int uPID, int fd, char *username, Node **head){


  Node *temp = *head;
  char t_username[21];
  strncpy(t_username, username, 20);

  //find data user needs based on given ID
  //print that player data

  Player *play = calloc(1, sizeof(Player));

  while(temp != NULL){
    if(temp->userid == uPID) {
      readp(fd, temp->index, play);
      break;
    }else{
      temp = temp->next;
    }
  }
  return play;
}
