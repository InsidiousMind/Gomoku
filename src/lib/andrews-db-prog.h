
typedef struct player {
  int userid;
  char first[20];
  char last[20];
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


