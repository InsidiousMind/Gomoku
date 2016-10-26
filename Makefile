#
# Makefile for the Gomoku project.
# 
#

make:
	mkdir build
	mkdir build/server
	cc src/server/server.c -g -Wall -Wextra -o build/server/server

clean:
	rm -rf build

linux:


server:
