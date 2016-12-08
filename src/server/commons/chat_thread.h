//
// Created by insi on 12/8/16.
//
#include "server_connections.h"

#ifndef SERVER_CHAT_THREAD_H
#define SERVER_CHAT_THREAD_H
typedef struct chat_args {
  //the head of our linkedlist of socket connections
  c_head *conn_head;
  pthread_mutex_t conn_head_access;
  Node *db_head;
  pthread_mutex_t db_head_access;
  bool stop;
  int db_fd;
} chatArgs;
void chat_subserver(void *args);
#endif //SERVER_CHAT_THREAD_H
