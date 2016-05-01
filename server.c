#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#define ACK "1"
#define MAX 2048

void serv_func(int sockfd, struct sockaddr_in *pcliaddr, socklen_t clilen) {
	int n;
	char sendline[MAX] = {0}, recvline[MAX] = {0};

	recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pcliaddr, &clilen);
	sendto(sockfd, ACK, 1, 0, (struct sockaddr *) pcliaddr, clilen);
	printf("%s\n", recvline);
	
}

int main() {
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
	serv_func(sockfd, &cliaddr, sizeof(cliaddr));

	return 0;
}
