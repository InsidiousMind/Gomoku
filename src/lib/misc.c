#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int isNextArg();
int readWord(char **tmp, int size);
int flush(FILE *where);

void INThandle(int sig){

  char c;

  signal(sig, SIG_IGN);

  printf(" NO.\n");
  printf("OMG..LIKE..NOOOO.. DID YOU?... oh no...");
  printf("DID YOU REAAAAALLLLLLYY...like REALLLLYY just hit Ctrl-C?\n ");
  printf("Do you REALLY want to quit? [y/n]");

  c = getchar();

  if(c == 'y' || c == 'Y')
    exit(0);
  else
    signal(SIGINT, INThandle);
  //grab the newline char so it doesn't screw stuff up
  getchar();
}



int* readInts(int count){
  int *numArr = calloc(count,sizeof(*numArr));

  int i;
 
  for(i = 0; i < count; i++){
    readWord(( (char**)&numArr[i], sizeof(int)));
  }
  flush(stdin);
  return numArr;  
}

//String MUST be malloced when using this function
int readWord(char **tmp, int size) {
  char *str = *tmp; 

  // Allocate memory for a string.
  // avoid buffer overflow through realloc

	char c;
	int i = 0;

	do {
		c = fgetc(stdin);
	  
    //reallocate memory by 1B every time
    //it goes over the size (initially 1B)
		if (i >= size) {
			size++;
			*tmp = realloc(*tmp, sizeof(char) * size);
          str = *tmp;
		}

    //stops at a whitespace to get entire word
    if (c == '\n' || c == EOF || c == ' ') {
		  str[i]	= '\0';
      break;
		}else{
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
  while (((c = fgetc(where)) != '\n') && (c != EOF))  /*abyss of doesn't matter */;
  return c;
}
