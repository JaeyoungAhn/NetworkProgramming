#include <stdio.h> // for printf() and fprintf()
#include <sys/socket.h> // for socket(), connect(), sendto(), and recvfrom()
#include <arpa/inet.h> // for sockaddr_in and inet_addr()
#include <stdlib.h> // for aton() and exit()
#include <string.h> // for memset() 
#include <unistd.h> // for close()

#define ECHOMAX 255 // longest string to echo

int main(int argc, char *agv[])
{
	int sock;
	struct sockaddr_in echoServAddr; // Echo Server address
	struct sockaddr_in fromAddr; // Source address of echo
	unsigned short echoServPort=9909; // Echo server port
	unsigned int fromSize; //address size for recvfrom()
	char *servIP="127.0.0.1";
	char *echoString="I hope this works";
	/* String to sent to echo server */
	char echoBuffer[ECHOMAX+1];
	/* Buffer for receiving echoed string */
	int echoStringLen;
	int respStringLen;
	
	/* Create a datagram/UDP socket */
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	/* Zero out structure */
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(servIP);
	/* Server IP address */
	echoServAddr.sin_port = htons(echoServPort);
	/* Server port */

	/* Send the string to the server */
	sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));

	/* Recv a response */
	fromSize = sizeof(fromAddr);
	recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
	/* Error checks like packet is received from the same server*/

	/* null-terminate the received data  */
	echoBuffer[echoStringLen]='\0';
	printf("Received: %s\n", echoBuffer); /* Print the echoed arg */
	close(sock);
	exit(0);
} /* end of main() */

