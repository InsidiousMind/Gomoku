# Gomoku Makefile


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# MACROS
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

CC = cc
CFLAGS = -Wextra -Wall -lpthread -g -Wshadow

#Server

SRV_SRC = src/server/
SRV_DEP = src/server/commons/
SRV_OBJ = main.o asgn6-server.o game_thread.o server_db.o server_connections.o chat_thread.o
BUILD_SRV_OBJ = build/server/main.o build/server/asgn6-server.o build/server/game_thread.o build/server/server_db.o build/server/server_connections.o build/server/chat_thread.o

#Client

CLIENT_SRC = src/client
CLIENT_DEP = src/client/commons/
CLIENT_OBJ = asgn6-client.o IO_sighandle.o
BUILD_CLIENT_OBJ = build/client/asgn6-client.o build/lib/IO_sighandle.o

#Dependencies

LIB_SRC = src/lib/
DEP_OBJ = gips.o glogic.o database.o client_connect.o
BUILD_DEP_OBJ = build/lib/gips.o build/lib/glogic.o build/lib/database.o build/lib/client_connect.o


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# MAKE RULES
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
make: dir server client curses

dir:
	mkdir -p build/lib/
	mkdir -p build/server/
	mkdir -p build/client/
	mkdir -p build/bin/

server: $(SRV_OBJ) $(DEP_OBJ)
	$(CC) -o build/bin/server $(BUILD_SRV_OBJ) $(BUILD_DEP_OBJ) $(CFLAGS)

client: $(CLIENT_OBJ) $(DEP_OBJ)
	$(CC) -o build/bin/client $(BUILD_CLIENT_OBJ) $(BUILD_DEP_OBJ) $(CFLAGS)

curses:
	cp src/client/curses-client.py build/bin/

all: dir server client curses

clean:
	rm -rf build
	rm -rf vgcore.*
	rm -rf test
	rm -rf src/client/*.txt
	rm -rf *.txt
	rm -rf src/client/CursesClient/*.pyc
	rm -rf src/client/CursesClient/__pycache__/


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# OBJECT COMPILATION
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

#Server Objects

main.o: $(DEP_OBJ) $(SRV_SRC)main.c $(SRV_DEP)asgn6-server.h
	$(CC) -c src/server/main.c -o build/server/main.o $(CFLAGS)

asgn6-server.o: $(SRV_DEP)asgn6-server.c $(SRV_DEP)game_thread.h $(SRV_DEP)asgn6-server.h $(SRV_DEP)chat_thread.h
	$(CC) -c src/server/commons/asgn6-server.c -o build/server/asgn6-server.o $(CFLAGS)

game_thread.o: $(SRV_DEP)game_thread.c $(SRV_DEP)server_db.h $(LIB_SRC)gips.h $(LIB_SRC)glogic.h $(SRV_DEP)game_thread.h
	$(CC) -c src/server/commons/game_thread.c -o build/server/game_thread.o $(CFLAGS)

#Client Objects

asgn6-client.o: $(LIB_SRC)IO_sighandle.h $(LIB_SRC)gips.h
	$(CC) -c src/client/asgn6-client.c -o build/client/asgn6-client.o $(CFLAGS)

#Library Objects

gips.o: $(LIB_SRC)gips.c $(LIB_SRC)gips.h
	$(CC) -c src/lib/gips.c -o build/lib/gips.o $(CFLAGS)

glogic.o: $(LIB_SRC)glogic.c $(LIB_SRC)gips.h $(LIB_SRC)glogic.h
	$(CC) -c src/lib/glogic.c -o build/lib/glogic.o $(CFLAGS)

IO_sighandle.o: $(LIB_SRC)IO_sighandle.c $(LIB_SRC)IO_sighandle.h
	$(CC) -c src/lib/IO_sighandle.c -o build/lib/IO_sighandle.o $(CFLAGS)

database.o: $(LIB_SRC)database.c
	$(CC) -c src/lib/database.c -o build/lib/database.o $(CFLAGS)

client_connect.o: $(CLIENT_DEP)client_connect.c
	$(CC) -c src/client/commons/client_connect.c -o build/lib/client_connect.o $(CFLAGS)

server_db.o: $(LIB_SRC)database.h $(LIB_SRC)gips.h $(SRV_DEP)server_db.h
	$(CC) -c src/server/commons/server_db.c -o build/server/server_db.o $(CFLAGS)

server_connections.o: $(SRV_DEP)server_connections.h
	$(CC) -c src/server/commons/server_connections.c -o build/server/server_connections.o $(CFLAGS)

chat_thread.o: $(LIB_SRC)database.h $(SRV_DEP)chat_thread.h
	$(CC) -c src/server/commons/chat_thread.c -o build/server/chat_thread.o $(CFLAGS)


