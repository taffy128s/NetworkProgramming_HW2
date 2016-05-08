CC = g++ -Wall -std=c++11

compile:
	$(CC) -o server server.cpp
	$(CC) -o client client.cpp
