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

void cli_func(int sockfd, struct sockaddr_in *pservaddr, socklen_t servlen) {
	int n;
	char sendline[MAX] = {0}, recvline[MAX] = {0};
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 2000;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockfd, &set);

	sprintf(sendline, "hello");
	sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);

	while (select(sockfd + 1, &set, NULL, NULL, &tv)) {
		if ((n = recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pservaddr, &servlen)) <= 0)
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pservaddr, servlen);
		else break;
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
