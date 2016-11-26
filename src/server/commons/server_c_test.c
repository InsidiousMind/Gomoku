//Dynamic Linked List implementation for storing connections to the server
//the head stores information about the Linked List
//so far, all it stores is the size

//To Compile:
//
//gcc server_c_test.c server_connections.c -I .
// (or cc, whatever)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "server_connections.h"

int main(){
  c_head *head = NULL;
  
  add(&head, 1);

  update(&head, 2);
  del(&head, 1);
  add(&head, 3);
  add(&head, 2);
  del(&head, 2);

  return 0;
}


