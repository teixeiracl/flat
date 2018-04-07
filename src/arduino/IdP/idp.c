#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../cofee.h"

void logexit(const char *str) {
	if (errno)
		perror(str);
	else
		puts(str);
	exit(EXIT_FAILURE);
}

// char *client = "192.168.1.170";
// char *sp = "192.168.1.162";
char *client = "192.168.0.16";
char *sp = "192.168.0.18";

int client_port = 12333;
int sp_port = 80;

void process_msg(char *received);

void send_msg(char *msg, char *address, int port);

int main(int argc, char **argv) {
	uint32_t counter = 0;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in clt,
			   srv = {.sin_family = AF_INET,
				   .sin_port = htons(51515),
				   .sin_addr.s_addr = htonl(INADDR_ANY)
				};

	// Creating socket and binding
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		logexit("socket");

	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		logexit("setsockopt(SO_REUSEADDR) failed");
	
	if (bind(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0)
		logexit("binding");

	while (1) {
		int addrlen = sizeof(clt);

		fprintf(stdout, "waiting for connection\n");
		listen(sock, 1);

		int newsock = accept(sock, (struct sockaddr *)&clt, &addrlen);
		if (newsock == -1)
			logexit("accept");

		int nrcv = recv(newsock, buffer, BUFFER_SIZE, MSG_WAITALL);
		if (nrcv == 0)
			logexit("recv");

		send(newsock, ACK, BUFFER_SIZE, 0);

		close(newsock);

		process_msg(buffer);
	}
	close(sock);

	exit(EXIT_SUCCESS);
}

void process_msg(char *received)
{
	if (strcmp(received, KEY_REQ) == 0) {
		fprintf(stdout, "Received KEY_REQ.\n");
		send_msg(KEY_REP, client, client_port);
		send_msg(CERT_PLAIN, sp, sp_port);
	}
	else if (strcmp(received, CERT_SIG) == 0) {
		fprintf(stdout, "Received CERT_SIG.\n");
		send_msg(SHARE_KEY, sp, sp_port);
	}
	else if (strcmp(received, ASSERT_REQ) == 0) {
		fprintf(stdout, "Received ASSERT_REQ.\n");
		send_msg(ASSERT_REP, client, client_port);
	}
	else if (strcmp(received, ACK) == 0) {
		fprintf(stdout, "Received ACK.\n");
	}
	else {
		fprintf(stdout, "invalid type of message: %s\n", received);
	}
}

void send_msg(char *msg, char *address, int port)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1)
		logexit("socket");

	struct in_addr addr = { .s_addr = htonl(INADDR_ANY) };
	struct sockaddr_in dst;
	dst.sin_family = AF_INET,
	dst.sin_port = htons(port);

	if(inet_pton(AF_INET, address, &dst.sin_addr) <= 0)
		logexit("inet_pton error occured");

	struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	if(connect(s, sa_dst, sizeof(dst)))
		logexit("connect");

	send(s, msg, BUFFER_SIZE, 0);

	char buffer[BUFFER_SIZE];
	if(0 >= recv(s, buffer, BUFFER_SIZE, MSG_WAITALL))
		logexit("recv");
	
	fprintf(stdout, "Received %s\n", buffer);

	close(s);
}