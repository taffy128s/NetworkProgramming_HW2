#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <sstream>
#define MAX 2048

using namespace std;

string name;

void udpSend(int sockfd, const char *data, struct sockaddr *pservaddr, int servlen) {
	sendto(sockfd, data, strlen(data), 0, pservaddr, servlen);
}

int udpRecvfrom(int sockfd, string &data, struct sockaddr *pservaddr, socklen_t *servlen) {
	char temp[MAX] = {0};
	int retval = recvfrom(sockfd, temp, MAX, 0, pservaddr, servlen);
	data = temp;
	return retval;
}

void show_menu() {
	printf("**********Hello %s**********\n", name.data(m));
	printf("[D]elete Account\n");
}

void cli_func(int sockfd, struct sockaddr_in *pservaddr, socklen_t servlen) {
	stringstream ss;
	string stringSend, stringRecv, stringBuffer, command, flag;
	/* Set the timeout value. */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	/* Use select to resend data. */
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	/* Set socket recv timeout. */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	/* Client sends hello. */
	udpSend(sockfd, "HeLlO", (struct sockaddr *) pservaddr, servlen);
	/* Send this packet until it successfully receive message from server.*/
	while (1) {
		select(sockfd + 1, &set, NULL, NULL, &tv);
		if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
			udpSend(sockfd, "HeLlO", (struct sockaddr *) pservaddr, servlen);
		} else break;
	}
	/* Print the welcome message sent from server. */
	cout << stringRecv;
	/* Choose login or register. Wrong command will not be accepted. */
	getline(cin, command);
	if (command == "R") {
		udpSend(sockfd, "ReGiStEr", (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, "ReGiStEr", (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		cout << stringRecv;
		stringSend = "ReGiStErReQuEsT ";
		getline(cin, stringBuffer);
		stringSend += stringBuffer;
		udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		cout << stringRecv;
		/* Parse the information from server. */
		ss.str("");
		ss << stringBuffer;
		ss >> name;
		ss.str("");
		ss << stringRecv;
		ss >> flag;
		// If the registration fails, it will return;
		if (flag != "Registered") return;
	} else if (command == "L") {
		udpSend(sockfd, "LoGiN", (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, "LoGiN", (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		cout << stringRecv;
		stringSend = "LoGiNrEqUeSt ";
		getline(cin, stringBuffer);
		stringSend += stringBuffer;
		udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		cout << stringRecv;
		/* Parse the information from server. */
		ss.str("");
		ss << stringBuffer;
		ss >> name;
		ss.str("");
		ss << stringRecv;
		ss >> flag;
		// If Login fails, return;
		if (flag != "Login") return;
	} else {
		cout << "Command not found" << endl;
		exit(0);
	}
	while (1) {
		show_menu();
		cout << "Your input:" << endl;
		getline(cin, command);
		if (command == "L") {
			
		}
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc != 2) {
		fprintf(stderr, "usage: ./client <IPaddress>\n");
		exit(1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	cli_func(sockfd, &servaddr, sizeof(servaddr));

	return 0;
}