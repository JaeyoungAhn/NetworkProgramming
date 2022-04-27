#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ECHOMAX 255

int main(int argc, char *argv[])
{
	int sock; /* Socket */
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;
	char echoBuffer[ECHOMAX];
	unsigned short echoServPort=9909;
	int recvMsgSize;

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr=htonl("127.0.0.1");
	echoServAddr.sin_port = htons(echoServPort);

	bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));

	for(;;) {
		cliAddrLen = sizeof(echoClntAddr);

		recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*) &echoClntAddr, &cliAddrLen);

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

		sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr*)&echoClntAddr, sizeof(echoClntAddr));

	}
}
