int connect_to_server() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *serv;
	serv = gethostbyname(HOST);
	int succ = connect(sock, serv, sizeof(serv));
	if (succ != -1) {
		return sock;
	}
	return -1;
}
