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
#define MAX 4096

using namespace std;

string name;

void udpSend(int sockfd, const char *data, struct sockaddr *pservaddr, socklen_t servlen) {
	sendto(sockfd, data, strlen(data), 0, pservaddr, servlen);
}

int udpRecvfrom(int sockfd, string &data, struct sockaddr *pservaddr, socklen_t *servlen) {
	char temp[MAX] = {0};
	int retval = recvfrom(sockfd, temp, MAX, 0, pservaddr, servlen);
	data = temp;
	return retval;
}

void show_menu() {
	printf("**********Hello %s**********\n", name.data());
	printf("[D]elete Account\n");
	printf("[MP]Modify Profile\n");
	printf("[SP]Show Profile\n");
	printf("[SA]Show Article\n");
	printf("[EA]Edit Article\n");
	printf("[DA]Delete Article\n");
	printf("[A]dd Article\n");
	printf("[E]nter Article\n");
	printf("[C]hat\n");
	printf("[S]earch User\n");
	printf("[L]ogout\n");
}

void cli_func(int sockfd, struct sockaddr_in *pservaddr, socklen_t servlen) {
	stringstream ss;
	string stringSend, stringRecv, stringBuffer, command, flag;
	/* Set the timeout value. */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
	/* Use select to resend data. */
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	/* Set socket recv timeout. */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	/* Client sends hello. */
	udpSend(sockfd, "HeLlO\n", (struct sockaddr *) pservaddr, servlen);
	/* Send this packet until it successfully receive message from server.*/
	while (1) {
		select(sockfd + 1, &set, NULL, NULL, &tv);
		if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
			udpSend(sockfd, "HeLlO\n", (struct sockaddr *) pservaddr, servlen);
		} else break;
	}
	/* Print the welcome message sent from server. */
	cout << stringRecv;
	/* Choose login or register. Wrong command will not be accepted. */
	getline(cin, command);
	if (command == "R") {
		stringSend = "";
		stringRecv = "";
		stringBuffer = "";
		command = "";
		flag = "";
		udpSend(sockfd, "ReGiStEr\n", (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, "ReGiStEr\n", (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		cout << stringRecv;
		stringSend = "ReGiStErReQuEsT ";
		getline(cin, stringBuffer);
		stringSend += stringBuffer + "\n";
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
		stringSend = "";
		stringRecv = "";
		stringBuffer = "";
		command = "";
		flag = "";
		udpSend(sockfd, "LoGiN\n", (struct sockaddr *) pservaddr, servlen);
		while (1) {
			select(sockfd + 1, &set, NULL, NULL, &tv);
			if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
				udpSend(sockfd, "LoGiN\n", (struct sockaddr *) pservaddr, servlen);
			} else break;
		}
		/* Client print the information from the server. */
		cout << stringRecv;
		stringSend = "LoGiNrEqUeSt ";
		getline(cin, stringBuffer);
		stringSend += stringBuffer + "\n";
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
		stringSend = "";
		stringRecv = "";
		stringBuffer = "";
		command = "";
		flag = "";
		show_menu();
		cout << "Your input:" << endl;
		getline(cin, command);
		if (command == "L") {
			stringSend = "";
			stringSend = name + " LoGoUt\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
			ss.str("");
			ss << stringRecv;
			ss >> flag;
			if (flag == "Logged") break;
		} else if (command == "D") {
			stringSend = "";
			stringSend = name + " DeLeTeAcCoUnT\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
			ss.str("");
			ss << stringRecv;
			ss >> flag;
			if (flag == "Delete") break;
		} else if (command == "MP") {
			stringSend = "";
			printf("Please enter your nickname and birthday. EX: taffy 1/1\n");
			getline(cin, stringBuffer);
			stringSend = name + " MoDiFyPrOfIlE " + stringBuffer + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "SP") {
			stringSend = "";
			stringSend = name + " ShOwPrOFiLe\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "S") {
			stringSend = "";
			printf("Input the account:\n");
			getline(cin, stringBuffer);
			stringSend = name + " SeArChAcCoUnT " + stringBuffer + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
			ss.str("");
			ss << stringRecv;
			ss >> flag;
			if (flag == "Found.") {
				printf("Add friend? (y/n)\n");
				string friendtoadd = stringBuffer;
				getline(cin, stringBuffer);
				if (stringBuffer == "y") {
					stringSend = name + " AdDfRiEnD " + friendtoadd + "\n";
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
					while (1) {
						select(sockfd + 1, &set, NULL, NULL, &tv);
						if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
							udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						} else break;
					}
					cout << stringRecv;
				}
				printf("Delete friend? (y/n)\n");
				getline(cin, stringBuffer);
				if (stringBuffer == "y") {
					stringSend = name + " DeLeTeFrIeNd " + friendtoadd + "\n";
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
					while (1) {
						select(sockfd + 1, &set, NULL, NULL, &tv);
						if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
							udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						} else break;
					}
					cout << stringRecv;
				}
			}
		} else if (command == "C") {
			stringSend = "";
			stringSend = name + " ShOwFrIeNdS\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
			printf("Talk to somebody or group or not? (y/g/n)\n");
			command = "";
			getline(cin, command);
			if (command == "y") {
				printf("Enter the name:\n");
				string nametosend;
				getline(cin, nametosend);
				printf("You can type 'q' to exit, 'g' to get message.\n");
				while (1) {
					string msg;
					getline(cin, msg);
					if (msg == "q") break;
					else if (msg == "g") {
						stringSend = "";
						stringSend = name + " GeTmEsSaGe " + nametosend + "\n";
						udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						while (1) {
							select(sockfd + 1, &set, NULL, NULL, &tv);
							if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
								udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
							} else break;
						}
						cout << stringRecv;
					} else {
						stringSend = "";
						stringSend = name + " SeNdMeSsAgE " + nametosend + " " + msg + "\n";
						udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						while (1) {
							select(sockfd + 1, &set, NULL, NULL, &tv);
							if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
								udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
							} else break;
						}
						cout << stringRecv;
					}
				}
			} else if (command == "g") {
				printf("Enter the group name:\n");
				string grptosend;
				getline(cin, grptosend);
				printf("You can type 'q' to exit, 'g' to get message.\n");
				while (1) {
					string msg;
					getline(cin, msg);
					if (msg == "q") break;
					else if (msg == "g") {
						stringSend = "";
						stringSend = name + " GeTmEsSaGeGrP " + grptosend + "\n";
						udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						while (1) {
							select(sockfd + 1, &set, NULL, NULL, &tv);
							if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
								udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
							} else break;
						}
						cout << stringRecv;
					} else {
						stringSend = "";
						stringSend = name + " SeNdMeSsAgEgRp " + grptosend + " " + msg + "\n";
						udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
						while (1) {
							select(sockfd + 1, &set, NULL, NULL, &tv);
							if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
								udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
							} else break;
						}
						cout << stringRecv;
					}
				}
			}
		} else if (command == "SA") {
			stringSend = "";
			stringSend = name + " ShOwArTiClE\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "A") {
			printf("Your new article:\n");
			string article;
			getline(cin, article);
			stringSend = "";
			stringSend = name + " NeWaRtIClE " + article + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "E") {
			printf("input <username> <numofarticle> (start from 0).\n");
			string temp;
			getline(cin, temp);
			stringSend = "";
			stringSend = name + " EnTeRaRtIcLe " + temp + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "EA") {
			string temp, edited;
			printf("Enter the article num you want to edit (start from 0):\n");
			getline(cin, temp);
			printf("Enter the new content:\n");
			getline(cin, edited);
			stringSend = "";
			stringSend = name + " EdItArTiClE " + temp + " " + edited + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else if (command == "DA") {
			string temp;
			printf("Enter the article num you want to delete (start from 0):\n");
			getline(cin, temp);
			stringSend = "";
			stringSend  = name + " DeLeTeArTiClE " + temp + "\n";
			udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
			while (1) {
				select(sockfd + 1, &set, NULL, NULL, &tv);
				if (udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pservaddr, &servlen) <= 0) {
					udpSend(sockfd, stringSend.data(), (struct sockaddr *) pservaddr, servlen);
				} else break;
			}
			cout << stringRecv;
		} else {
			cout << "Command not found.\n";
		}
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc != 3) {
		fprintf(stderr, "usage: ./client <IPaddress> <port>\n");
		exit(1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	cli_func(sockfd, &servaddr, sizeof(servaddr));

	return 0;
}
