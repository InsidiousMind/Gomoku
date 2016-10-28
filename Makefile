#
# Makefile for the Gomoku project.
# 
#

make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	cc src/server/server.c -g -lpthread -o build/server/server
	cc src/client/linux/client.c -g -o build/client/linux/client

client:
	mkdir -p build
	mkdir -p build/client
	cc src/client/client.c -o build/client/client

server:
	mkdir -p build
	mkdir -p build/server
	cc src/server/server.c -g -lpthread -Wall -Wextra -o build/server/server
	ln -s build/server/server server


debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/client
	cc src/server/server.c -g -lpthread -Wall -Wextra -o debug/server/server
	cc src/client/client.c -g -Wall -Wextra -o debug/client/client

client-debug:
	mkdir -p debug
	mkdir -p debug/client
	cc src/client/client.c -g -Wall -Wextra -o debug/client/client

server-debug:
	mkdir -p debug
	mkdir -p debug/server
	cc src/server/server.c -g -lpthread -Wall -Wextra -o debug/server/server

clean:
	rm -rf build
	rm -rf debug
