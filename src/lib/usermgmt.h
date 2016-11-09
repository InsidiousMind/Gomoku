// This file will be used to handle user registration and
// logging in to the server.

// This should be used by the CLIENT to send newly-registered user or to
// send/recieve an existing user
// from the server.
int login();
// This will allow the server to receive the login/registration from the CLIENT
// and send a PID back to the player. The PID will not change, every time it's
// sent back it will be the same value in the database.
int server_add_user();
