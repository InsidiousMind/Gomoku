/*
 * Client utilizing the Gomoku Inter Process Shuttle (GIPS) protocol designed by Sean Batzel and Andrew Plaza.
 *
 * Client designed by Sean Batzel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include "../../gips/gips.h"

#define HOST "server1.cs.scranton.edu"
#define HTTPPORT "32200"
#define BACKLOG 10

int send_to_server() {

}

int receive_from_server() {

}

void display_board(int board1, int board2) {

}

int connect_to_server() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *serv;
	serv = gethostbyname(HOST);
	int succ = connect(sock, serv, sizeof(serv));
	if (succ != -1) {
		return sock;
	}
	return -1;
}

int main() {
	char board[8][8];
	char *name;
	int move_x;
	int move_y;
	int sock = connect_to_server();
	printf("Gomoku Client for Linux\n");
	if (sock != -1) {
		printf("Enter your name: ");
		scanf("%s", name);
		send(sock, name, sizeof(name), 0);
	} else {
		printf("Couldn't connect to the server.\n");
		exit(0);
	}
	while (board1 != 999 && board2 != 999) {
		printf("%d> ", name);
		// Asyncronously make moves.
	}
	close(sock);
}
