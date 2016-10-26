make:
	mkdir build
	mkdir build/server
	mkdir build/client
	mkdir build/client/linux
	cc src/server/server.c -g -Wall -Wextra -o build/server/server
	cc src/client/linux/client.c -g -Wall -Wextra -o build/client/linux/client

clean:
	rm -rf build
