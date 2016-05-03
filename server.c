#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#define ACK "1"
#define MAX 2048

void serv_func(int sockfd, struct sockaddr_in *pcliaddr, socklen_t clilen) {
	DIR *dp;
	struct dirent *ep;
	char sendline[MAX], recvline[MAX], command[MAX];
	/* Receive messages from client. */
	while (1) {
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		memset(command, 0, sizeof(command));
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
		} else if (!strcmp("LoGiNrEqUeSt", command)) {
			printf("%s", recvline);
		} else if (!strcmp("ReGiStErReQuEsT", command)) {
			int found = 0;
			char username[100] = {0}, password[100] = {0};
			/* Parse the username and password. */
			sscanf(recvline, "%*s%s%s", username, password);
			/* If there's no password entered, send error message to client. */
			if (strlen(password) == 0) {
				sprintf(sendline, "Password cannot be null.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
				continue;
			}
			/* Try to find out whether there exists a file with the same name. */
			dp = opendir("./data/user/");
			if (dp != NULL) {
				while ((ep = readdir(dp))) {
					if (!strcmp(username, ep->d_name)) {
						found = 1;
						break;
					}
				}
				closedir(dp);
			}
			/* If there's a file with the same name, send error message. */
			if (found) {
				sprintf(sendline, "Username is either used or registered successfully, please connect again and login.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
				continue;
			} else {
				char path[100] = {0};
				sprintf(path, "./data/user/");
				strcat(path, username);
				//printf("%s\n", path);
				FILE *fp = fopen(path, "wb");
				fprintf(fp, "%s", password);
				fclose(fp);
				sprintf(sendline, "Registered successfully.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
			}
		}
	}
}

int main() {
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	mkdir("./data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./data/user", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
	serv_func(sockfd, &cliaddr, sizeof(cliaddr));

	return 0;
}
