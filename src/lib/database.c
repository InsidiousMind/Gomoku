/*
 * Author: Sean Batzel
 * Player database consisting of an ID number, last name, first name, number of wins, number of losses, and number of ties.
 * Modified to add commands to modify the list during runtime.
 * Modified to add file persistence to the database.
 * Modified to remove the "delete" command.
 * Compile: cc asgn1-batzels4.c -o asgn4
 * Input:
 *  Add a new player:	+ userid lastname firstname wins losses ties
 *	Update existing:	* userid wins losses ties
 *	Find a player:		? userid
 *	Stop the program:	# <prints all entries>
 * Note:
 *  If a userid already exists and is added to the database, output will be "ERROR - userid exists."
 *	If a userid does not exist and is queried, or updated, output will be "ERROR - player does not exist."
 */

#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

typedef struct player {
	int userid;
	char first[20];
	char last[20];
	int wins;
	int losses;
	int ties;
} Player;

typedef struct node {
	int player_id;
	int index;
	struct node *next;
} Node;

void insert(int id, int fd, Player *player, Node **head) {
	printf("ADD %d, %s, %s, %d, %d, %d\n", player->userid, player->last, player->first, player->wins, player->losses, player->ties);
	Node *tmp = *head;
	if (tmp != NULL) {
		if (tmp->player_id == id) {
			printf("Player ID already exists.\n");
			free(tmp);
			return;
		}
		while (tmp != NULL) {
			if (tmp->player_id == id) {
				printf("Player ID already exists.\n");
				free(tmp);
				return;
			} else {
				tmp = tmp->next;
			}
		}
	}
	free(tmp);
	Node *x = (Node *)malloc(sizeof(Node));
	x->player_id = id;
	int offset = lseek(fd, 0, SEEK_END);
	x->index = offset;
	if ((*head) != NULL && (*head)->player_id == id) {
		printf("ERROR - Player ID already exists.\n");
		free(x);
		return;
	}
	else if (*head == NULL || (*head)->player_id > x->player_id) {
		x->next = *head;
		*head = x;
		write(fd, player, sizeof(Player));
	} else {
		tmp = *head;
		while (tmp->next !=NULL && tmp->next->player_id <= x->player_id) {
			if (x->player_id == tmp->next->player_id) {
				printf("ERROR - Player ID already exists.\n");
				free(x);
				return;
			}
			tmp =  tmp->next;
		}
		x->next = tmp->next;
		tmp->next = x;
		write(fd, player, sizeof(Player));
	}
}

void update(int id, int fd, Player *player, Node *head) {
	Node *tmp = head;
	while (tmp->next != NULL && tmp->player_id != id) {
		tmp = tmp->next;
	}
	if (tmp->player_id == id) {
		Player *tmp2 = (Player *)malloc(sizeof(Player));
		lseek(fd, tmp->index, SEEK_SET);
		read(fd, tmp2, sizeof(Player));
		printf("BEFORE: %d, %s, %s, %d, %d, %d\n", tmp2->userid, tmp2->last, tmp2->first, tmp2->wins, tmp2->losses, tmp2->ties);
		tmp2->wins = player->wins;
		tmp2->losses = player->losses;
		tmp2->ties = player->ties;
		printf("AFTER: %d, %s, %s, %d, %d, %d\n", tmp2->userid, tmp2->last, tmp2->first, tmp2->wins, tmp2->losses, tmp2->ties);
		lseek(fd, tmp->index, SEEK_SET);
		write(fd, tmp2, sizeof(Player));
		free(tmp2);
	} else {
		if (tmp->next == NULL) {
			printf("ERROR - Player does not exist.\n");
		}
	}
}

void query(int id, int fd, Node *head) {
	Node *tmp = head;
	while (tmp != NULL) {
		if (tmp->player_id == id) {
			Player *tmp2 = (Player *)malloc(sizeof(Player));
			lseek(fd, tmp->index, SEEK_SET);
			read(fd, tmp2, sizeof(Player));
			printf("QUERY: %d, %s, %s, %d, %d, %d\n", tmp2->userid, tmp2->last, tmp2->first, tmp2->wins, tmp2->losses, tmp2->ties);
			free(tmp2);
			return;
		} else {
			tmp = tmp->next;
		}
	}
	printf("ERROR - Player does not exist.\n");
}

void print_list(Node *head) {
	printf("The linked list: ");
	Node *tmp = head;
	while (tmp != NULL) {
		printf("(%d, %d), ", tmp->player_id, tmp->index);
		tmp = tmp->next;
	}
	printf("\n");
}

void print_file(int fd) {
	printf("Players:\n");
	lseek(fd, 0, SEEK_SET);
	Player *p = (Player *)malloc(sizeof(Player));
	while (read(fd, p, sizeof(Player)) != 0) {
		printf("%d, %s, %s, %d, %d, %d\n", p->userid, p->last, p->first, p->wins, p->losses, p->ties);
	}
	free(p);
	printf("End\n");
}

Player *create_player() {
	Player *x = (Player *)malloc(sizeof(Player));
	scanf("%d%s%s%d%d%d", &(x->userid), x->first, x->last, &(x->wins), &(x->losses), &(x->ties));
	return x;
}

Player *create_player_up() {
	Player *x = (Player *)malloc(sizeof(Player));
	scanf("%d%d%d%d", &(x->userid), &(x->wins), &(x->losses), &(x->ties));
	return x;
}

void readp(int fd, int index, Player *play){
	lseek(fd, index*sizeof(Player),0);
	if(read(fd, play, sizeof(Player)) == -1)
		die("[ERROR] read failed");
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
		Node *newn = (Node *)malloc(sizeof(Node));
		memset(newn,0,sizeof(*newn));

		readp(fd, temp_i, &rec);
		newn->userid = rec.userid;
		newn->index = temp_i;
		insert(&temp, newn);
		temp_i++;
	}
	*index = temp_i;
	*head = temp;
}

