#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "database.h"
#include "gips.h"

int isNextArg();
int readWord(void *tmp, int size, int isInt);
int flush(FILE *where);

void INThandle(int sig){
  Node *head; 
 
  char c;

  signal(sig, SIG_IGN);
  write(0, "Do you really want to quit? [Y/n]", 33);

  c = getchar();

  if(c == 'y' || c == 'Y')
    exit(0);
  else
    signal(SIGINT, INThandle);
  //grab the newline char so it doesn't screw stuff up
  getchar();
}

void free_LL(Node**head){
  Node *node = *head;
  Node *temp;
  while(node != NULL){
    temp = node;
    node = node->next;
    temp->next = NULL;
    free(temp);
  }
  *head = NULL;

}
/*int* readInts(int count){
  int *numArr = calloc(count,sizeof(*numArr));

=======
int* readInts(int count){
>>>>>>> 062bff9f1a9def2a0d225714b0aea956d07e9d9e
  int i;

  int *numArr = calloc(count,sizeof(int*));
   
  for(i = 0; i < count; i++){
    readWord((void*)&numArr[i], 4, TRUE);
  }
  //flush(stdin);
  return numArr;  
}*/

//String MUST be malloced when using this function
int readWord(void *tmp, int size, int isInt) {

  // Allocate memory for a string.
  // avoid buffer overflow through realloc
  char *str = tmp;
	char c;
	int i = 0;

	do {
		c = fgetc(stdin);
	  
    //reallocate memory by 1B every time
    //it goes over the size (initially 1B)
		if (i >= size) {
			size++;
			tmp = realloc(tmp, sizeof(char) * size);
      str = tmp;
		}

  //check if Int 
  //it's a str
  //stops at a whitespace to get entire word
  
    if (c == '\n' || c == EOF || c == ' ') {
      if(isInt == TRUE) break;
      else{
        str[i]	= '\0';
        break;
      }
    }else{
      if(isInt == TRUE) str[i] = (int)c;
      else
        str[i] = c;
      i++;
    }
	} while (1);
  
  if (c == ' '){

    ungetc(c, stdin);
    //this is for getting rid of whitespace
    return isNextArg();

  }else
    return 0;
}

//check if the next character is the start of another
//argument, and not simply whitespace
int isNextArg(){
  char c;
  int i = 0;

  do{
    c = fgetc(stdin);
    i++;
  } while (c == ' ');
  
  if(c == EOF || c == '\n' )
    return 0;
  else{
    ungetc(c, stdin);
    return -1;
  }
}

int flush(FILE *where){
  int c;
  while (((c = fgetc(where)) != '\n') || (c != EOF))  /*abyss of null*/;
  return c;
}
