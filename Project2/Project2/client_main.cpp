#include "client_header.h"

#define RCVBUFSIZE 256
#define PORTNUM 5500
#define ERR_EXIT { \
	fprintf(stderr, "ERROR: %ld\n", WSAGetLastError()); \
	WSACleanup(); \
	system("pause"); \
	return 0; \
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET c_sock;
	int msg_len;
	char buffer[RCVBUFSIZE];

	string s, r;
	//vector<string> res;
	struct sockaddr_in c_addr;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	c_sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (c_sock == INVALID_SOCKET)
		ERR_EXIT;
	memset((char *)&c_addr, 0, sizeof(c_addr));
	c_addr.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &c_addr.sin_addr.s_addr);
	c_addr.sin_port = htons(PORTNUM);

	if (connect(c_sock, (SOCKADDR*)&c_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		closesocket(c_sock);
		cout << "Connected Error." << endl;
		ERR_EXIT;
	}
	fprintf(stdout, "Please give your massage: \n");
	while (fgets(buffer, RCVBUFSIZE, stdin)) {
		if (send(c_sock, buffer, strlen(buffer), 0) < 0) {
			closesocket(c_sock);
			cout << "Send message Error." << endl;
			ERR_EXIT;
		}
		if (strncmp(buffer, "EXIT\n", 5) == 0) {
			fprintf(stdout, "Normal Exit\n");
			break;
		}
		if (strncmp(buffer, "BOUNCE ", 7) == 0) {
			fprintf(stdout, "Please give your massage:\n");
			continue;
		}
		if (strncmp(buffer, "EXIT ", 5) == 0) {
			fprintf(stdout, "EXIT\n");
			break;
		}
		if (strncmp(buffer, "GET ", 4) == 0) {
			char buf[RCVBUFSIZE];
			while (strcmp(buffer, "-1") != 0) {
				memset(buffer, 0, RCVBUFSIZE);
				if (msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0) < 0)
					ERR_EXIT;
				if (msg_len = send(c_sock, "get context", 11, 0) < 0)
					ERR_EXIT;
				if (strcmp(buffer, "-1") == 0)
					break;
			}
		}
		else {
			memset(buffer, 0, RCVBUFSIZE);
			if (msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0) < 0)
				ERR_EXIT;
			fprintf(stdout, "%s\n", buffer);
		}
		fprintf(stdout, "Please give your massage:\n");
	}
	closesocket(c_sock);
	system("pause");
	return 0;
}
