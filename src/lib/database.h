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
int query(int id, int fd, Node *head, int verbose);
Player *get_player_by_name(char *username, int id, int fd, Node *head, int verbose);
Player *create_player();
Player *create_player_up();
void print_list(Node *head);
void print_file(int fd);


