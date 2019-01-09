#include "server_header.h"

#define MAXPENDING 5
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
	SOCKET rv_sock, s_sock;
	int msg_len;
	char server_buf[RCVBUFSIZE], client_buf[RCVBUFSIZE];
	struct sockaddr_in serv_addr;
	sockaddr_in client_addr;
	FILE *fp;
	bool flag = false;
	//if (argc != 2) { /* Test for correct number of arguments */
	//	fprintf(stdout, "Usage: %s server_port\n", argv[0]);
	//	return 0;
	//}
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	rv_sock = WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (rv_sock == INVALID_SOCKET)
		ERR_EXIT;
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORTNUM);
	if (bind(rv_sock, (SOCKADDR*)&serv_addr,
		sizeof(serv_addr)) == SOCKET_ERROR) {
		closesocket(rv_sock);
		ERR_EXIT;
	}
	if (listen(rv_sock, MAXPENDING) == SOCKET_ERROR) {
		closesocket(rv_sock);
		ERR_EXIT;
	}
	
	while (1) {
		int len = sizeof(client_addr);
		fprintf(stdout, "\nWaiting for client to connect...\n");
		//if (s_sock = accept(rv_sock, NULL, NULL) == INVALID_SOCKET)
			//ERR_EXIT;
		s_sock = accept(rv_sock, (SOCKADDR*)&client_addr, &len);
		/*if (flag) {
			send(s_sock, "Cannot accept one more client.", 30, 0);
			ERR_EXIT;
		}*/
		flag = true;
		printf("Now %s is connected.\n", inet_ntop(AF_INET, &client_addr.sin_addr, client_buf, sizeof(client_buf)));
		
		while (flag) {
			memset(server_buf, 0, RCVBUFSIZE);
			if (msg_len = recv(s_sock, server_buf, RCVBUFSIZE - 1, 0)  < 0)
				break;
			if (strncmp(server_buf, "GET ", 4) == 0) {
				char filename[RCVBUFSIZE] = { 0 };
				char buf[RCVBUFSIZE];
				for (int i = 5; i < strlen(server_buf) - 2; i++)
					filename[i - 5] = server_buf[i];
				FILE* file;
				fopen_s(&file, filename, "rt");
				if (!file) {
					fprintf(stdout, "Cannot open %s\n", filename);
					if (msg_len = send(s_sock, "ERROR: no such file.", 20, 0) < 0)
						ERR_EXIT;
					if (msg_len = recv(s_sock, buf, RCVBUFSIZE - 1, 0) < 0)
						ERR_EXIT;
					if (msg_len = send(s_sock, "-1", 2, 0) < 0)
						ERR_EXIT;
					if (msg_len = recv(s_sock, buf, RCVBUFSIZE - 1, 0) < 0)
						ERR_EXIT;
				}
				else {
					while (fgets(buf, RCVBUFSIZE, file)) {
						fprintf(stdout, "%s\n", filename);
						if (msg_len = send(s_sock, buf, strlen(buf), 0) < 0)
							ERR_EXIT;
						if (msg_len = recv(s_sock, buf, RCVBUFSIZE - 1, 0) < 0)
							ERR_EXIT;
					}
					if (msg_len = send(s_sock, "-1", 2, 0) < 0) {
						fclose(file);
						ERR_EXIT;
					}
					if (msg_len = recv(s_sock, buf, RCVBUFSIZE - 1, 0) < 0)
						ERR_EXIT;
				}
			}
			else if (strncmp(server_buf, "BOUNCE ", 7) == 0) {
				char buf[RCVBUFSIZE] = { 0 };
				for (int i = 8; i < strlen(server_buf) - 2; i++)
					buf[i - 8] = server_buf[i];
				fprintf(stdout, "%s\n", buf);
			}
			else if (strncmp(server_buf, "EXIT ", 5) == 0) {
				char buf[RCVBUFSIZE] = { 0 };
				for (int i = 6; i < strlen(server_buf) - 2; i++)
					buf[i - 6] = server_buf[i];
				fprintf(stdout, "Client exit with code <%s>\n", buf);
				closesocket(s_sock);
				flag = false;
			}
			else if (strcmp(server_buf, "EXIT\n") == 0) {
				fprintf(stdout, "Client common exit\n");
				closesocket(s_sock);
				flag = false;
			}
			else
				if (msg_len = send(s_sock, "Please check your command.\n", 25, 0) < 0)
					ERR_EXIT;

		}
	}
	closesocket(s_sock);

	system("pause");
	return 0;
}