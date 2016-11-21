CC=gcc
CFLAGS=-std=c99 -o myshell

all: myshell.c
	$(CC) $(CFLAGS) myshell.c
	
