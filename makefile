CC = g++ -Wall

compile:
	$(CC) -o server server.cpp
	$(CC) -o client client.cpp