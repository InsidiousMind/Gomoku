#
# Makefile for the Gomoku project.
# 
#

<<<<<<< HEAD
CC = gcc
CCO = gcc -c
CDEBUG = -g -Wall -Wextra -Werror
SERV_SRC = src/server/server.c

=======
CC = cc
CCO = cc -c
CDEBUG = -g -Wall -Wextra
>>>>>>> af6c83d6dc5b0d85552e932e42611c2141ea8069
make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	$(CC) $(SERV_SRC) -lpthread -o build/server/server
	$(CC) src/client/client.c -o build/client/client

client:
	mkdir -p build
	mkdir -p build/client
	$(CC) src/client/client.c -o build/client/client

server:
	mkdir -p build
	mkdir -p build/server
	$(CC) $(SERV_SRC) -lpthread -o build/server/server


debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/client
	$(CC) $(SERV_SRC) $(CDEBUG) -lpthread  -o debug/server/server
	$(CC) src/client/client.c $(CDEBUG) -o debug/client/client

client-debug:
	mkdir -p debug
	mkdir -p debug/client
	$(CC) src/client/client.c $(CDEBUG) -o debug/client/client

server-debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/lib/
<<<<<<< HEAD
	${CCO} $(SERV_SRC) $(CDEBUG) -lpthread -o debug/server/server.o
=======
>>>>>>> af6c83d6dc5b0d85552e932e42611c2141ea8069
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	${CCO} src/server/server.c $(CDEBUG) -lpthread -o debug/server/server.o
	${CC} debug/server/server.o debug/lib/gips.o debug/lib/network.o $(CDEBUG) -lpthread -o debug/server/server

clean:
	rm -rf build
	rm -rf debug
