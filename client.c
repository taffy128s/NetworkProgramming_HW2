#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#define MAX 2048

char name[100];

void show_menu() {
	printf("**********Hello %s**********\n", name);
}

void cli_func(int sockfd, struct sockaddr_in *pservaddr, socklen_t servlen) {
	char sendline[MAX] = {0}, recvline[MAX] = {0}, buffer[MAX] = {0};
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
	sprintf(sendline, "HeLlO");
	sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
	/* Send this packet until it successfully receive message from server.*/
	while (1) {
		select(sockfd + 1, &set, NULL, NULL, &tv);
		if (recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen) <= 0) {
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
			/*printf("tried\n");*/
		} else break;
	}
	/* Print the welcome message sent from server. */
	printf("%s", recvline);
	memset(sendline, 0, sizeof(sendline));
	memset(recvline, 0, sizeof(recvline));
	memset(buffer, 0, sizeof(buffer));
	/* Choose login or register. Wrong command will not be accepted. */
	fgets(sendline, MAX, stdin);
	if (sendline[0] == 'R') {
		sprintf(sendline, "ReGiStEr");
		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		printf("%s", recvline);
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		memset(buffer, 0, sizeof(buffer));
		sprintf(sendline, "ReGiStErReQuEsT ");
		fgets(buffer, MAX, stdin);
		strcat(sendline, buffer);
		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		printf("%s", recvline);
		/* Parse the information from server. */
		char flag[100] = {0};
		sscanf(buffer, "%s", name);
		sscanf(recvline, "%s", flag);
		if (strcmp("Registered", flag)) return;
	} else if (sendline[0] == 'L') {
		sprintf(sendline, "LoGiN");
		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		printf("%s", recvline);
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		memset(buffer, 0, sizeof(buffer));
		sprintf(sendline, "LoGiNrEqUeSt ");
		fgets(buffer, MAX, stdin);
		strcat(sendline, buffer);
		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		printf("%s", recvline);
		/* Parse the information from server. */
		char flag[100] = {0};
		sscanf(buffer, "%s", name);
		sscanf(recvline, "%s", flag);
		if (strcmp("Login", flag)) return;
	} else {
		printf("Command not found.\n");
		exit(0);
	}
	memset(sendline, 0, sizeof(sendline));
	memset(recvline, 0, sizeof(recvline));
	memset(buffer, 0, sizeof(buffer));
	show_menu();
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
