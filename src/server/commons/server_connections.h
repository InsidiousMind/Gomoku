//
// Created by insi on 11/24/16.
//

#ifndef SERVER_SERVER_CONNECTIONS_H
#define SERVER_SERVER_CONNECTIONS_H
typedef struct connectionList {
  int sockfd;
  int isPlaying;
  struct connectionList *next;
} cList;

void add(cList **head, int sockfd);
void update(cList **head, int sockfd);



#endif //SERVER_SERVER_CONNECTIONS_H
