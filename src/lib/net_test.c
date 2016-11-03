#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "gips.h"
#include "network.h"

int send_to(gips *info, int sock) {
        int total = 0;
        int bytesleft = sizeof(info), n;
        int len = sizeof(info);
        while (total < len) {
                n = send(sock, &(*(info + total)), bytesleft, 0);
                if (n == -1) {
                        perror("[!!!] could not send");
                        break;
                }
                total += n; //tally up what was sent
                bytesleft -= n; //bytes left to send
        }
        len = total; // number of bytes actually sent
        return n == -1 ? -1 : 0; //-1 on fail 0 on success
}
