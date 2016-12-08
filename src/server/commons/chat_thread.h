//
// Created by insi on 12/8/16.
//
#include "server_connections.h"
#include "../../lib/database.h"

#ifndef SERVER_CHAT_THREAD_H
#define SERVER_CHAT_THREAD_H
typedef struct chat_args {
  //the head of our linkedlist of socket connections
  c_head *conn_head;
  Node *db_head;
  head
  bool stop;
  int db_fd;
} chatArgs;

void poll_for_chat(void *args);
#endif //SERVER_CHAT_THREAD_H
