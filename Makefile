#
# Makefile for the Gomoku project.
# 
#

CC = cc
CCO = cc -c
CDEBUG = -g -Wall -Wextra -Werror
SERV_SRC = src/server/server.c
DBG_DEPS =  debug/lib/gips.o debug/lib/network.o 

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
	mkdir -p debug/lib/
	$(CCO) src/client/client.c $(CDEBUG) -o debug/client/client.o
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	$(CC) debug/client/client.o $(DBG_DEPS) $(CDEBUG) -o debug/client/client

server-debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/lib/
	${CCO} $(SERV_SRC) $(CDEBUG) -lpthread -o debug/server/server.o
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	$(CCO) src/lib/glogic.c $(CDEBUG) -o debug/lib/glogic.o
	${CCO} src/server/server.c $(CDEBUG) -lpthread -o debug/server/server.o
	${CC} debug/server/server.o $(DBG_DEPS) $(CDEBUG) -lpthread -o debug/server/server

clean:
	rm -rf build
	rm -rf debug
