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

int  translate_to_gips(char *line) {
	
}

char *translate_from_gips(int gips) {

}

int send_gips_to_server(int gips) {

}

int receive_gips_from_server(int gips) {

}

int display_board() {

}

int main() {
	// board will store the gips-formatted board.
	int board = 0;
	printf("Gomoku Client for Linux\n");
	// Try connecting here.
	/* If connected, send a user id (firstname lastname)
	 * to the server.
	 */
	// Display the board.
	// Begin accepting/sending input.
	// Accept a command line.
	// Process the command.
	// Send to the server.
	// Receive the server's response.
	// Display the board.
	// Loop back.
	return 0;
} 
