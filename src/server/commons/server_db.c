#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "../../lib/gips.h"

#include "../../lib/database.h"

void initp(Player **player, int userid, char *username, int wins, int losses, int ties);
int sendp(Player **play, int sockfd);

//record player data to player's entry in struct
//if player doesn't exist, create an entry


Player **recPlayer(uint32_t uPID, BYTE PID, char *username, int isWin, Node *head, int fd)
{

    static Player *player;
    bool p_exist = doesPlayerExist(&head, uPID, username);
    //if the player doesn't exist create a new player record, and add it
    //else just update the existing player record
    if((! p_exist) && isWin != -1) {
        player = calloc(1, sizeof(Player));
        initp(&player, uPID, username, isWin == PID, !(isWin == PID), 0);
        head = add(fd, getIndex(fd), &head, &player);
    }else if((! p_exist) && (isWin == -1)) {
        player = calloc(1, sizeof(Player));
        initp(&player, uPID, username, 0, 0, 0);
        head = add(fd, getIndex(fd), &head, &player);
    }else if(isWin == -1){/*do nothing */;
    }else update(fd, &head, uPID, isWin==PID, !(isWin == PID), 0);

    player = getPlayer(uPID, fd, username, &head);

    return &player;
}

int sendPlayer(uint32_t uPID, char *username, Node *head, int sockfd, int fd)
{
    Player *player;

    if(doesPlayerExist(&head, uPID, username) == false)
    {
        player = calloc(1, sizeof(Player));
        initp(&player, uPID, username, 0, 0,0);
        head = add(fd, getIndex(fd), &head, &player);
    }

    player = getPlayer(uPID, fd, username, &head);
    if(sendp(&player, sockfd) == -1) return -1;
    else return 0;

}

void player_htonl(Player **play){
    Player *player = *play;

    player->userid = htonl(player->userid);
    player->wins = htonl(player->wins);
    player->losses = htonl(player->losses);
    player->ties = htonl(player->ties);
    player->index = htonl(player->index);
    *play = player;

}

void player_ntohl(Player **play){
    Player *player = *play;

    player->userid = ntohl(player->userid);
    player->wins = ntohl(player->wins);
    player->losses = ntohl(player->losses);
    player->ties = ntohl(player->losses);
    player->index = ntohl(player->index);
    *play = player;
}

int sendp(Player **play, int sockfd){
    Player *player = *play;

    player_htonl(&player);
    if(send(sockfd, player, sizeof(Player), 0) == -1){
        free(player);
        return -1;
    } else {
        free(player);
        return 0;
    }
}

//index is set in update/add
//just an initializer function (pass by ref)
void initp(Player **player, int userid, char *username, int wins, int losses, int ties){
    Player *play = *((Player **) player);

    play->userid = userid;
    strncpy(play->username, username, 20);
    play->userid = userid;
    play->wins = wins;
    play->losses = losses;
    play->ties = ties;
}

