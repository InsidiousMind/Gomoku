#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include "database.h"
#include "gips.h"


int isNextArg();
int toDigit(char c);


//same as readWord but for ints
int readInts(int *numArr, int size, int *i){

  char c;
  int *tmp = numArr;

  do {
    //realloc memory if necessary
    c = getchar();
    while((c == '\n') && (*i == 0)) c = getchar();

    if (*i >= size) {
      size++;
      numArr = realloc(numArr, sizeof(int) * size);
      tmp = numArr;
    }

    //stops at a whitespace to get the entire word
    if (c == '\n' || c == EOF || c == ' ') break;
    else{
      if(isdigit(c)){
        int a = toDigit(c);
        if(a != -1) tmp[*i] = a;
        ++(*i);
      } else{ /* do nothing */ }
    }

  } while(TRUE);

  if (c == ' ' || c == '\n'){

    ungetc(c, stdin);

    //this is for getting rid of whitespace
    return isNextArg();

  }else return FALSE;
}

int toDigit(char c){

  int i;
  char dArr[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  for(i = 0; i < 9; i++) {
    if(c == dArr[i]) return i+1;
  }
  return -1;
}

//String MUST be malloced when using this function
int readWord(char *tmp, int size) {

  // Allocate memory for a string.
  // avoid buffer overflow through realloc
  char c;
  int i = 0;
  char *str = tmp;

  do {
    c = getchar();

    //reallocate memory by 1B every time
    //it goes over the size (initially 1B)
    if (i >= size) {
      size++;
      tmp = realloc(tmp, sizeof(char) * size);
      str = tmp;
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

  }else return FALSE;
}

//check if the next character is the start of another
//argument, and not simply whitespace
int isNextArg(){
  char c;
  int i = 0;

  do{
    c = getchar();
    i++;
  } while (c == ' ');

  if(c == EOF || c == '\n' )
    return FALSE;
  else ungetc(c, stdin);
  return TRUE;
}
