#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <ctime>
#define ACK "1"
#define MAX 4096

using namespace std;

map<string, bool> onlineStatus;
map<pair<string, string>, bool> friendrequests;
map<string, double> offlinetimee;
map<pair<string, string>, string> msgs;
map<string, string> grpmsgs; 
struct timeval t;
double timee;
map<string, vector<string> > articles; 

void udpSend(int sockfd, const char *data, struct sockaddr *paddr, int len) {
	sendto(sockfd, data, strlen(data), 0, paddr, len);
}

int udpRecvfrom(int sockfd, string &data, struct sockaddr *paddr, socklen_t *len) {
	char temp[MAX] = {0};
	int retval = recvfrom(sockfd, temp, MAX, 0, paddr, len);
	data = temp;
	return retval;
}

void serv_func(int sockfd, struct sockaddr_in *pcliaddr, socklen_t clilen) {
	DIR *dp;
	struct dirent *ep;
	char sendline[MAX];
	stringstream ss;
	string stringSend, stringRecv, stringCommand, temp, name;
	/* Receive messages from client. */
	while (1) {
		ss.str("");
		stringCommand = "";
		stringSend = "";
		stringRecv = "";
		stringCommand = "";
		temp = "";
		name = "";
		memset(sendline, 0, sizeof(sendline));
		udpRecvfrom(sockfd, stringRecv, (struct sockaddr *) pcliaddr, &clilen);
		cout << stringRecv;
		ss << stringRecv;
		ss >> stringCommand;
		if (stringCommand == "HeLlO") {
			sprintf(sendline, "**********Welcome**********\n[R]egister [L]ogin\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		} else if (stringCommand == "LoGiN") {
			sprintf(sendline, "Please enter your account and password. Ex: hello 123456\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		} else if (stringCommand == "ReGiStEr") {
			sprintf(sendline, "Please enter new account and password. Ex: hello 123456\n");
			sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
		} else if (stringCommand == "LoGiNrEqUeSt") {
			int found = 0;
			char username[100] = {0}, password[100] = {0};
			/* Parse the username and password. */
			sscanf(stringRecv.data(), "%*s%s%s", username, password);
			/* If there's no password entered, send error massage to client. */
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
				char path[100] = {0}, rightPasswd[100] = {0};
				sprintf(path, "./data/user/");
				strcat(path, username);
				FILE *fp = fopen(path, "rb");
				fscanf(fp, "%s", rightPasswd);
				fclose(fp);
				if (!strcmp(rightPasswd, password)) {
					temp = username;
					onlineStatus[temp] = true;
					sprintf(sendline, "Login successfully!\n");
					sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
				} else {
					sprintf(sendline, "Wrong password.\n");
					sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
				}
			} else {
				sprintf(sendline, "Account not found.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
			}
		} else if (stringCommand == "ReGiStErReQuEsT") {
			int found = 0;
			char username[100] = {0}, password[100] = {0};
			/* Parse the username and password. */
			sscanf(stringRecv.data(), "%*s%s%s", username, password);
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
				sprintf(sendline, "Username is either used or registered successfully.\nPlease connect again and login.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
			} else {
				char path[100] = {0};
				sprintf(path, "./data/user/");
				strcat(path, username);
				FILE *fp = fopen(path, "wb");
				fprintf(fp, "%s", password);
				fclose(fp);
				string stringusername = username;
				string stringpath = "./data/friend/" + stringusername;
				mkdir(stringpath.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				temp = username;
				onlineStatus[temp] = true;
				sprintf(sendline, "Registered successfully.\n");
				sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) pcliaddr, clilen);
			}
		} else {
			char username[100] = {0}, command[100] = {0};
			string stringUsername;
			sscanf(stringRecv.data(), "%s%s", username, command);
			stringUsername = username;
			stringCommand = command;
			if (stringCommand == "DeLeTeAcCoUnT") {
				string path = "./data/user/";
				path += stringUsername;
				remove(path.data());
				onlineStatus[stringUsername] = false;
				gettimeofday(&t, NULL);
				timee = t.tv_sec * 1000.0;
				timee += t.tv_usec / 1000.0;
				offlinetimee[stringUsername] = timee / 1000;
				udpSend(sockfd, "Delete account successfully. Logged out.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "LoGoUt") {
				onlineStatus[stringUsername] = false;
				gettimeofday(&t, NULL);
				timee = t.tv_sec * 1000.0;
				timee += t.tv_usec / 1000.0;
				offlinetimee[stringUsername] = timee / 1000;
				udpSend(sockfd, "Logged out.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "MoDiFyPrOfIlE") {
				char nickname[100] = {0}, bd[100] = {0};
				sscanf(stringRecv.data(), "%*s%*s%s%s", nickname, bd);
				string stringNickname = nickname;
				string stringBd = bd;
				string path = "./data/profile/";
				path += stringUsername;
				ofstream file;
				file.open(path.data());
				file << stringNickname;
				file << " ";
				file << stringBd;
				file.close();
				udpSend(sockfd, "Change Profile successfully.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "ShOwPrOFiLe") {
				string path = "./data/profile/";
				path += stringUsername;
				ifstream file;
				file.open(path.data());
				getline(file, stringSend);
				stringSend += "\n";
				file.close();
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "SeArChAcCoUnT") {
				int found = 0;
				char nametofind[100] = {0};
				sscanf(stringRecv.data(), "%*s%*s%s", nametofind);
				string stringnametofind = nametofind;
				if (stringnametofind == stringUsername) {
					temp = "Please do not enter your own account.\n";
					udpSend(sockfd, temp.data(), (struct sockaddr *) pcliaddr, clilen);
					continue;
				}
				dp = opendir("./data/user/");
				if (dp != NULL) {
					while ((ep = readdir(dp))) {
						if (!strcmp(stringnametofind.data(), ep->d_name)) {
							found = 1;
							break;
						}
					}
					closedir(dp);
				}
				if (found) {
					temp = "Found. Name is: " + stringnametofind + "\n";
					udpSend(sockfd, temp.data(), (struct sockaddr *) pcliaddr, clilen);
				} else {
					temp = "User not found.\n";
					udpSend(sockfd, temp.data(), (struct sockaddr *) pcliaddr, clilen);
				}
			} else if (stringCommand == "AdDfRiEnD") {
				char nametoadd[100] = {0};
				sscanf(stringRecv.data(), "%*s%*s%s", nametoadd);
				string stringnametoadd = nametoadd;
				pair<string, string> thepair(stringnametoadd, stringUsername);
				if (friendrequests[thepair]) {
					friendrequests[thepair] = false;
					string path1 = "./data/friend/" + stringUsername + "/" + stringnametoadd;
					string path2 = "./data/friend/" + stringnametoadd + "/" + stringUsername;
					ofstream file1, file2;
					file1.open(path1.data());
					file2.open(path2.data());
					file1.close();
					file2.close();
					udpSend(sockfd, "Add friend successfully!\n", (struct sockaddr *) pcliaddr, clilen);
				} else {
					pair<string, string> pairr(stringUsername, stringnametoadd);
					friendrequests[pairr] = true;
					udpSend(sockfd, "Friend request sent!\n", (struct sockaddr *) pcliaddr, clilen);
				}
			} else if (stringCommand == "DeLeTeFrIeNd") {
				char nametodel[100] = {0};
				sscanf(stringRecv.data(), "%*s%*s%s", nametodel);
				string stringnametodel = nametodel;
				pair<string, string> pair1(stringUsername, stringnametodel);
				pair<string, string> pair2(stringnametodel, stringUsername);
				friendrequests[pair1] = false;
				friendrequests[pair2] = false;
				string path1 = "./data/friend/" + stringUsername + "/" + stringnametodel;
				string path2 = "./data/friend/" + stringnametodel + "/" + stringUsername;
				remove(path1.data());
				remove(path2.data());
				udpSend(sockfd, "Delete friend successfully!\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "ShOwFrIeNdS") {
				string stringpath = "./data/friend/" + stringUsername + "/";
				dp = opendir(stringpath.data());
				stringSend = "\n";
				if (dp != NULL) {
					while ((ep = readdir(dp))) {
						temp = ep->d_name;
						if (temp == "." || temp == "..") continue;
						if (onlineStatus[temp]) {
							stringSend += temp + " is online.\n";
						} else {
							if (offlinetimee[temp] == 0) {
								stringSend += temp + " is never online.\n";
							} else {
								gettimeofday(&t, NULL);
								timee = t.tv_sec * 1000.0;
								timee += t.tv_usec / 1000.0;
								timee /= 1000;
								ostringstream strs;
								strs << ((timee - offlinetimee[temp]) / 60);
								string temptime = strs.str();
								stringSend += temp + " is offline for " + temptime + " minutes.\n";
							}
						}
					}
					closedir(dp);
				}
				stringSend += "\n";
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "GeTmEsSaGe") {
				char nametosend[100] = {0};
				string stringnametosend;
				sscanf(stringRecv.data(), "%*s%*s%s", nametosend);
				stringnametosend = nametosend;
				pair<string, string> p(stringUsername, stringnametosend);
				udpSend(sockfd, msgs[p].data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "SeNdMeSsAgE") {
				char nametosend[100] = {0}, message[100] = {0};
				string stringnametosend, stringmessage;
				sscanf(stringRecv.data(), "%*s%*s%s%s", nametosend, message);
				stringnametosend = nametosend, stringmessage = message;
				pair<string, string> p1(stringUsername, stringnametosend);
				pair<string, string> p2(stringnametosend, stringUsername);
				msgs[p1] += stringUsername + ": " + stringmessage + "\n";
				msgs[p2] += stringUsername + ": " + stringmessage + "\n";
				udpSend(sockfd, "Successfully send the msg.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "GeTmEsSaGeGrP") {
				char grptosend[100] = {0};
				string stringgrptosend;
				sscanf(stringRecv.data(), "%*s%*s%s", grptosend);
				stringgrptosend = grptosend;
				udpSend(sockfd, grpmsgs[stringgrptosend].data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "SeNdMeSsAgEgRp") {
				char grptosend[100] = {0}, message[100] = {0};
				string stringgrptosend, stringmessage;
				sscanf(stringRecv.data(), "%*s%*s%s%s", grptosend, message);
				stringgrptosend = grptosend, stringmessage = message;
				grpmsgs[grptosend] = stringUsername + ": " + stringmessage + "\n";
				udpSend(sockfd, "Successfully send the msg.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "NeWaRtIClE") {
				string article;
				article += "Account: " + stringUsername + "\n";
				string addr, port;
				addr = inet_ntoa(pcliaddr->sin_addr);
				port = to_string(ntohs(pcliaddr->sin_port));
				article += "IP: " + addr + ", port: " + port + "\n";
				article += "Content:";
				ss.str("");
				ss << stringRecv;
				ss >> temp >> temp;
				while (ss >> temp) {
					article += " " + temp;
				}
				article += "\n";
				time_t ticks;
				ticks = time(NULL);
				string timeeee = ctime(&ticks);
				article += "Time: " + timeeee;
				articles[stringUsername].push_back(article);
				udpSend(sockfd, "Successfully add the article.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "ShOwArTiClE") {
				string stringpath = "./data/friend/" + stringUsername + "/";
				dp = opendir(stringpath.data());
				stringSend = "\n";
				stringSend += stringUsername + " has " + to_string(articles[stringUsername].size()) + " articles.\n";
				if (dp != NULL) {
					while ((ep = readdir(dp))) {
						temp = ep->d_name;
						if (temp == "." || temp == "..") continue;
						stringSend += temp + " has " + to_string(articles[temp].size()) + " articles\n";
					}
					closedir(dp);
				}
				stringSend += "\n";
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "EnTeRaRtIcLe") {
				char namee[100] = {0};
				int number;
				sscanf(stringRecv.data(), "%*s%*s%s%d", namee, &number);
				string target = namee;
				stringSend = "";
				stringSend = articles[namee][number] + "\n";
				udpSend(sockfd, stringSend.data(), (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "DeLeTeArTiClE") {
				int idx;
				sscanf(stringRecv.data(), "%*s%*s%d", &idx);
				articles[stringUsername].erase(articles[stringUsername].begin() + idx);
				udpSend(sockfd, "Delete successfully.\n", (struct sockaddr *) pcliaddr, clilen);
			} else if (stringCommand == "EdItArTiClE") {
				int idx;
				sscanf(stringRecv.data(), "%*s%*s%d", &idx);
				string stringidx(to_string(idx));
				string article;
				article += "Account: " + stringUsername + "\n";
				string addr, port;
				addr = inet_ntoa(pcliaddr->sin_addr);
				port = to_string(ntohs(pcliaddr->sin_port));
				article += "IP: " + addr + ", port: " + port + "\n";
				article += "Content: ";
				char data[1000] = {0};
				int i, j = 0;
				for (i = stringUsername.size() + 1 + stringCommand.size() + 1 + stringidx.size() + 1; i < (int) stringRecv.size(); i++) {
					data[j++] = stringRecv[i];
				}
				temp = data;
				article += temp;
				time_t ticks;
				ticks = time(NULL);
				string timeeee = ctime(&ticks);
				article += "Time: " + timeeee;
				articles[stringUsername][idx] = article;
				udpSend(sockfd, "Successfully edit the article.\n", (struct sockaddr *) pcliaddr, clilen);
			}
		}
	}
}

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	if (argc != 2) {
		printf("usage: ./runnable <port>\n");
		exit(0);
	}

	mkdir("./data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./data/user", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./data/profile", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./data/friend", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
	serv_func(sockfd, &cliaddr, sizeof(cliaddr));

	return 0;
}
