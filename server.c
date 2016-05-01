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
	char sendline[MAX] = {0}, recvline[MAX] = {0}, command[MAX] = {0};
	/* Receive messages from client. */
	while (1) {
		recvfrom(sockfd, recvline, MAX, 0, (struct sockaddr *) pcliaddr, &clilen);
		sscanf(recvline, "%s", command);
		if (!strcmp("HeLlO", command)) {
			sprintf(sendline, "**********Welcome**********\n[R]egister [L]ogin\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		} else if (!strcmp("LoGiN", command)) {
			sprintf(sendline, "Please enter your account and password. Ex: hello 123456\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		} else if (!strcmp("ReGiStEr", command)) {
			sprintf(sendline, "Please enter new account and password. Ex: hello 123456\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		}
		
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		memset(command, 0, sizeof(command));
	}
}

int main() {
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	mkdir("./Server", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./Server/User", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
	serv_func(sockfd, &cliaddr, sizeof(cliaddr));

	return 0;
}
