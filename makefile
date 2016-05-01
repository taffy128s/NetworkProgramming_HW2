CC = gcc -Wall

compile:
	$(CC) -o server server.c
	$(CC) -o client client.c