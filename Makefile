#
# Makefile for the Gomoku project.
# 
#

make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	mkdir -p build/client/linux
	cc src/server/server.c -g -lpthread -Wall -Wextra -o build/server/server 
	cc src/client/linux/client.c -g -Wall -Wextra -o build/client/linux/client

linux:
	mkdir -p build
	mkdir -p build/client
	mkdir -p build/client/linux
	cc src/client/linux/client.c -g -Wall -Wextra -o build/client/linux/client

server:
	mkdir -p build
	mkdir -p build/server
	cc src/server/server.c -g -lpthread -Wall -Wextra -o build/server/server

clean:
	rm -rf build
