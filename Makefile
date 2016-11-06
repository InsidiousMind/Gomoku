# Gomoku Makefile


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# MACROS
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -lpthread

#Server

SRV_SRC = src/server/
SRV_OBJ = main.o asgn6-server.o game_thread.o
BUILD_SRV_OBJ = build/server/main.o build/server/asgn6-server.o build/server/game_thread.o

#Client

CLIENT_SRC = src/client
CLIENT_OBJ = asgn6-client.o
BUILD_CLIENT_OBJ = build/client/asgn6-client.o

#Dependencies

LIB_SRC = src/lib/
DEP_OBJ = gips.o glogic.o network.o
BUILD_DEP_OBJ = build/lib/gips.o build/lib/glogic.o build/lib/network.o


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# MAKE RULES
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
dir: 
	mkdir -p build/lib/
	mkdir -p build/server/
	mkdir -p build/client/

server: $(SRV_OBJ) $(DEP_OBJ)
	$(CC) -o build/server/server $(BUILD_SRV_OBJ) $(BUILD_DEP_OBJ) $(CFLAGS)

client: $(CLIENT_OBJ) $(DEP_OBJ)
	$(CC) -o build/client/client $(BUILD_CLIENT_OBJ) $(BUILD_DEP_OBJ) $(CFLAGS)

all: server client

clean: 
	rm -rf build


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\///
# OBJECT COMPILATION
# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//

#Server Objects

main.o: $(SRV_SRC)main.c $(SRV_SRC)asgn6-server.h
	$(CC) -c src/server/main.c -o build/server/main.o

asgn6-server.o: $(SRV_SRC)asgn6-server.c $(LIB_SRC)network.h $(SRV_SRC)game_thread.h $(SRV_SRC)asgn6-server.h 
	$(CC) -c src/server/asgn6-server.c -o build/server/asgn6-server.o

game_thread.o: $(LIB_SRC)gips.h $(LIB_SRC)glogic.h $(LIB_SRC)network.h $(SRV_SRC)game_thread.h
	$(CC) -c src/server/game_thread.c -o build/server/game_thread.o

#Client Objects

asgn6-client.o: $(LIB_SRC)network.h $(LIB_SRC)gips.h 
	$(CC) -c src/client/asgn6-client.c -o build/client/asgn6-client.o

#Library Objects

gips.o: $(LIB_SRC)gips.c $(LIB_SRC)gips.h
	$(CC) -c src/lib/gips.c -o build/lib/gips.o

glogic.o: $(LIB_SRC)glogic.c $(LIB_SRC)gips.h $(LIB_SRC)glogic.h
	$(CC) -c src/lib/glogic.c -o build/lib/glogic.o

network.o: $(LIB_SRC)network.c $(LIB_SRC)gips.h $(LIB_SRC)network.h
	$(CC) -c src/lib/network.c -o build/lib/network.o
