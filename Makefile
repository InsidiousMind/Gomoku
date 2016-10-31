#
# Makefile for the Gomoku project.
#
#

CC = cc
CCO = cc -c
CDEBUG = -g -Wall -Wextra -Werror
SERV_SRC = src/server/server.c
CLIE_SRC = src/client/client.c
DBG_DEPS = debug/lib/gips.o debug/lib/network.o debug/lib/glogic.o
PRD_DEPS = build/lib/gips.o build/lib/network.o build/lib/glogic.o

make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
  $(CCO) $(CLIE_SRC) -o build/client/client.o
  $(CCO) $(SERV_SRC) -o build/server/server.o
	$(CCO) src/lib/gips.c -o build/lib/gips.o
	$(CCO) src/lib/network.c -o build/lib/network.o
	$(CC) $(PRD_DEPS) $(SERV_SRC) -lpthread -o build/server/server
	$(CC) $(PRD_DEPS) $(CLIE_SRC) -o build/client/client

client:
	mkdir -p build
	mkdir -p build/client	
	$(CC) $(CLIE_SRC) -o build/client/client

server:
	mkdir -p build
	mkdir -p build/server
	$(CC) $(SERV_SRC) -lpthread -o build/server/server


debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/client
	$(CC) $(SERV_SRC) $(CDEBUG) -lpthread  -o debug/server/server
	$(CC) $(CLIE_SRC) $(CDEBUG) -o debug/client/client

client-debug:
	mkdir -p debug
	mkdir -p debug/client
	mkdir -p debug/lib/
	$(CCO) $(CLIE_SRC) $(CDEBUG) -o debug/client/client.o
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
