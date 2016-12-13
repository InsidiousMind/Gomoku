/*
 *A multi-synchronous chat server using poll()
 * this server has a timeout of 100ms at which point if no event occured it will
 * call itself recursively. If a socket is ready to receive, however, it performs a
 * recv with MSG_PEEK to check if the first character is a \v
 * if it is, then it will take that message, parse it, and send it every other socket
 * connection in the list of socket descriptors
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <pthread.h>
#include <memory.h>
#include <errno.h>

//
// Created by insi on 12/8/16.
//

//server shared includes
#include "server_connections.h"
#include "../../lib/database.h"
#include "chat_thread.h"

char* concat(const char *s1, const char *s2) ;
int poll_for_chat(chatArgs *chatInfo);
// \v == chat mesg
void chat_subserver(void *args){
 
 chatArgs *chatInfo = ((chatArgs*) args);

  int ret = 0;
  while(chatInfo->stop != true) {
    ret = poll_for_chat(chatInfo);
  }
  if(chatInfo->stop) pthread_exit(NULL);
}
int poll_for_chat(chatArgs *chatInfo){

  char buf[1024];
  int i, rv;
  c_head *conn_head = chatInfo->conn_head;
  pthread_mutex_t conn_head_access = chatInfo->conn_head_access;
  pthread_mutex_t db_head_access = chatInfo->db_head_access;
   
   pthread_mutex_lock(&conn_head_access);
    struct pollfd ufds[conn_head->size];
   pthread_mutex_unlock(&conn_head_access);
  
  int head_size;
  pthread_mutex_lock(&conn_head_access);
  head_size = conn_head->size;
  pthread_mutex_unlock(&conn_head_access);
  
  while(head_size <= 1){
    usleep(250000); //250 milliseconds
    pthread_mutex_lock(&conn_head_access);
    head_size = conn_head->size;
    pthread_mutex_unlock(&conn_head_access);
  }
  
  //get an array of sockets from the linkedList keeping track of connections
  pthread_mutex_lock(&conn_head_access);
  int *sockets = getSockets(&conn_head);
  head_size = conn_head->size;
  pthread_mutex_unlock(&conn_head_access);
  
  //set events to watch for on each socket connection
  for(i = 0; i < head_size; i++){
    ufds[i].fd = sockets[i];
    ufds[i].events = POLLIN | POLLOUT;
  }
  // wait for events on the sockets, 1 second timeout
  rv = poll(ufds, head_size, 100);
  
  //-1 == error; 0 == timeout
  if(rv == -1)
  {
    perror("poll"); //error occurred in poll()
    return -1;
  } else if (rv == 0) {
    /*do nothing */
    return 0;
  } else {
    for (i = 0; i < conn_head->size; i++) {
      if (ufds[i].revents & POLLIN) { //data ready to be recved on this socket
        char peek;
        int read_count = (int) recv(ufds[i].fd, &peek, sizeof(char), MSG_PEEK | MSG_DONTWAIT);
        if(errno == EAGAIN || errno == EWOULDBLOCK )
          continue;
        if(peek == '\v'){
          read_count = recv(ufds[i].fd, &buf, sizeof(char) * 1024, 0);
          if(read_count == 0 || read_count == -1){
            perror("[!!!] recv error in chat_thread on socket");
            return -1;
          }else{
            int len_of_upid = (int)buf[1];
            char c_upid[len_of_upid]; //use str to long
            memmove(buf, buf+2, strlen(buf)); //get rid of first two ('\v' and len(c_upid) chars)
            memcpy(&c_upid, &buf, (size_t)len_of_upid);
            memmove(buf,buf+len_of_upid,strlen(buf));
            
            char* filling = ": \0";
            long uPID = strtol(c_upid, NULL, 10);
            Player *play;
            pthread_mutex_lock(&db_head_access);
            play = fpuPID(uPID, chatInfo->db_fd, &(chatInfo->db_head));
            pthread_mutex_unlock(&db_head_access);
            //form the message credentials
            char* leftP = "(\0";
            char* rightP =")\0";
            char* chat_msg_prefix = "\v\0";
            char* l_cupid = concat(leftP, c_upid); //free this
            char* parenth_cupid = concat(c_upid, rightP); //free this
            char* user_cupid =  concat(play->username, parenth_cupid); //free this
            char* msg_begin = concat(user_cupid, filling); //free this
            char* msg = concat(user_cupid, buf); //free this
            char *msg_finish = concat(chat_msg_prefix, msg);
            //send them to every socket in ufds
            free(l_cupid);
            free(parenth_cupid);
            free(user_cupid);
            free(msg_begin);
            free(msg);
            for(i = 0; i < head_size; i++) {
              /*do nothing*/
              if (send(ufds[i].fd, msg_finish, strlen(msg_finish), 0) == -1) {
                printf("could not send to socket; possible disconnect");
                pthread_mutex_lock(&conn_head_access);
                parseConnections(&conn_head);
                pthread_mutex_unlock(&conn_head_access);
                continue;
              }
            }
            free(msg_finish);
          }
        }
        //end of MSG_PEEK if
      }else if (ufds[i].revents & POLLOUT) {  //this socket is ready to send data
       //sucks, don't really have anything to put here
        
      //some kind of error or hangup occurred, close this socket
      }else if ((ufds[i].revents & POLLERR) || (ufds[i].revents & POLLHUP) ||
          (ufds[i].revents & POLLNVAL)) {
        //check if any errors occured
        pthread_mutex_lock(&conn_head_access);
        parseConnections(&conn_head);
        pthread_mutex_unlock(&conn_head_access);
      }
    } //end for loop
  }
  free(sockets);
  return 0;
}

//concatenates char s1 with char s2, so you get a string like
// S1S2
char* concat(const char *s1, const char *s2) {
  const size_t len1 = strlen(s1);
  const size_t len2 = strlen(s2);
  char *result = malloc(len1+len2+1);//+1 for the zero-terminator
  //in real code you would check for errors in malloc here
  memcpy(result, s1, len1);
  memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
  return result;
}



