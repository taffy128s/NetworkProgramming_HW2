CC = g++ -Wall -std=c++11

compile:
	$(CC) -o HW2_103062122_Ser server.cpp
	$(CC) -o HW2_103062122_Cli client.cpp
