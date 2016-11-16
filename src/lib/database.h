typedef struct player {
  int userid;
  char *username;
  int wins;
  int losses;
  int ties;
} Player;

typedef struct node {
  int player_id;
  int index;
  struct node *next;
} Node;

int insert(int id, int fd, Player *player, Node **head);
int update(int id, int fd, Player *player, Node *head);
Player *query(char *username, int id, int fd, Node *head, int verbose);
Player *create_player();
Player *create_player_up();
void print_list(Node *head);
void print_file(int fd);
void print_player(Player *player);

