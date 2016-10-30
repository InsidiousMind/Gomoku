#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include "gips.h"
#include "network.h"

/*int connect_to_server() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *serv;
	serv = gethostbyname(HOST);
	int succ = connect(sock, serv, sizeof(serv));
	if (succ != -1) {
		return sock;
	}
	return -1;
}*/

int send_to(gips *info, int sock) {


  int total = 0;
  int bytesleft = sizeof(info), n;
  int len = sizeof(info);
  while(total < len){
    n = send(sock, &(*(info+total)), bytesleft, 0);
    if(n==-1)
    {
      perror("[!!!] could not send");
      break;
    }
    total += n; //tally up what was sent
    bytesleft -= n; //bytes left to send
  }
  len = total; // number of bytes actually sent
  return n == -1? -1:0; //-1 on fail 0 on success
}

