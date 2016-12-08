<<<<<<< Updated upstream
void recPlayer(int uPID, BYTE PID, char *username, int isWin, Node *head, int sockfd, int fd);
=======
int recPlayer(uint32_t uPID, BYTE PID, char *username, int isWin, Node *head, int fd);
int sendPlayer(uint32_t uPID, char *username, Node *head, int fd, int sockfd);
int sendp(Player **play, int sockfd);
>>>>>>> Stashed changes



