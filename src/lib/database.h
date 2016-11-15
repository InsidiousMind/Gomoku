//should change these to reflect
//playerGameInfo

typedef struct player {
  int userid;
  char *first;
  char *last;
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
Player *get_player_by_name(char *username, int id, int fd, Node *head);
Player *create_player();
Player *create_player_up();
