#
# Makefile for the Gomoku project.
#
#

CC = cc
CCO = cc -c
CDEBUG = -g -Wall -Wextra
SERV_SRC = src/server/asgn6-server.c
CLIE_SRC = src/client/asgn6-client.c
DBG_DEPS = debug/lib/gips.o debug/lib/network.o debug/lib/glogic.o
PRD_DEPS = build/lib/gips.o build/lib/network.o build/lib/glogic.o

make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	mkdir -p build/lib
	$(CCO) $(CLIE_SRC) -o build/client/client.o
	$(CCO) $(SERV_SRC) -o build/server/server.o
	$(CCO) src/lib/gips.c -o build/lib/gips.o
	$(CCO) src/lib/network.c -o build/lib/network.o
	$(CCO) src/lib/glogic.c -o build/lib/glogic.o
	$(CC) $(PRD_DEPS) $(SERV_SRC) -lpthread -o build/server/server
	$(CC) $(PRD_DEPS) $(CLIE_SRC) -o build/client/client

client:
	mkdir -p build
	mkdir -p build/client
	mkdir -p build/lib
	$(CCO) $(CLIE_SRC) -o build/client/client.o
	$(CCO) src/lib/gips.c -o build/lib/gips.o
	$(CCO) src/lib/network.c -o build/lib/network.o
	$(CCO) src/lib/glogic.c -o build/lib/glogic.o
	$(CC) $(CLIE_SRC) $(PRD_DEPS) -o build/client/client

server:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/lib
	$(CCO) $(SERV_SRC) -o build/server/server.o
	$(CCO) src/lib/gips.c -o build/lib/gips.o
	$(CCO) src/lib/network.c -o build/lib/network.o
	$(CCO) src/lib/glogic.c -o build/lib/glogic.o
	$(CC) $(SERV_SRC) $(PRD_DEPS) -lpthread -o build/server/server


debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/client
	mkdir -p debug/lib
	$(CCO) $(CLIE_SRC) $(CDEBUG) -o debug/client/client.o
	$(CCO) $(SERV_SRC) $(CDEBUG) -o debug/server/server.o
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	$(CCO) src/lib/glogic.c $(CDEBUG) -o debug/lib/glogic.o
	$(CC) $(DBG_DEPS) $(SERV_SRC) $(CDEBUG) -lpthread  -o debug/server/server
	$(CC) $(DBG_DEPS) $(CLIE_SRC) $(CDEBUG) -o debug/client/client

client-debug:
	mkdir -p debug
	mkdir -p debug/client
	mkdir -p debug/lib/
	$(CCO) $(CLIE_SRC) $(CDEBUG) -o debug/client/client.o
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	$(CCO) src/lib/glogic.c $(CDEBUG) -o debug/lib/glogic.o
	$(CC) debug/client/client.o $(DBG_DEPS) $(CDEBUG) -o debug/client/client

server-debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/lib/
	${CCO} $(SERV_SRC) $(CDEBUG) -o debug/server/server.o
	$(CCO) src/lib/gips.c $(CDEBUG) -o debug/lib/gips.o
	$(CCO) src/lib/network.c $(CDEBUG) -o debug/lib/network.o
	$(CCO) src/lib/glogic.c $(CDEBUG) -o debug/lib/glogic.o
	${CCO} src/server/asgn6-server.c $(CDEBUG) -o debug/server/server.o
	${CC} debug/server/server.o $(DBG_DEPS) $(CDEBUG) -lpthread -o debug/server/server

run-client-debug:
	gdb debug/client/client

run-server-debug:
	gdb debug/server/server

run-client-valgrind:
	valgrind --leak-check=full -v --track-origins=yes debug/client/client

run-server-valgrind:
	valgrind --leak-check=full -v --track-origins=yes debug/server/server

clean:
	rm -rf build
	rm -rf debug
	rm -rf vgcore.*
