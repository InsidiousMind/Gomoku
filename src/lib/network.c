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

void send_to(long z, int sock) {
	send(sock, z, sizeof(z), 0);
}

long get_server(int sock) {
	long buf;
	recv(sock, buff, sizeof(buffer), 0);
	return buff;
}
