/*
 * Client utilizing the GIPS protocol designed by Sean Batzel and Andrew Plaza.
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

#define HOST "server1.cs.scranton.edu"
#define HTTPPORT "32200"
#define BACKLOG 10


int translate_to_gips(int x, int y) {
	
}

int translate_from_gips(int gips) {

}

int send_gips_to_server(int gips) {

}

int receive_gips_from_server() {

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
	int board1 = 0;
	int board2 = 0;
	char *name;
	int move_x;
	int move_y;
	int gips;
	int sock = connect_to_server();
	printf("Gomoku Client for Linux\n");
	if (sock != -1) {
		printf("Enter your name: ");
		scanf("%s", name);
	}
	while (board1 != 999 && board2 != 999) {
		display_board(board1, board2);
		printf("%s> ", name);
		scanf("%d %d", &move_x, &move_y);
		gips = translate_to_gips(move_x, move_y);
		send_gips_to_server(gips);
		board1 = receive_gips_from_server();
		board2 = receive_gips_from_server();
	}
	close(sock);
	return 0;
}
