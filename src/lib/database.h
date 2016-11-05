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

void insert(int id, int fd, Player *player, Node **head);
void update(int id, int fd, Player *player, Node *head);
void query(int id, int fd, Node *head);
void readp(int fd, int index, Player *play);
void persist(int fd, int *index, Node **head, char *filename);
