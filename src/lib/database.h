#include <stdbool.h>

typedef struct player {
  int userid;
  char username[20];
  int wins;
  int losses;
  int ties;
  int index;
} Player;


typedef struct node {
  int userid;
  int index;
  struct node *next;
} Node;


void persist(int fd, int *index, Node **head, char *filename);


void print_nodes(Node **head);
void print_players(int fd, Node **head);
void printp(int fd, int index);

void insert(Node **head, Node *newNode);

void query(int fd, Node **head);

bool doesPlayerExist(Node **head, int uPID, char *username);
bool isPlayerTaken(Node **head, int uPID, char *username, int fd);


int readnp(int fd, int index, Player *play);

int getIndex(int fd);

Node* add(int fd, int index, Node **head, int userid, 
    int wins, int losses, int ties, char* username);

Player* update(int fd, Node **head, int userid, int wins, int losses, int ties);

Player* getPlayer(int uPID, int fd, char *username, Node **head);




