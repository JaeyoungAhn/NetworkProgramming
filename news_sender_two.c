#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]){
	int send_sock;
	struct sockaddr_in* mul_adr;
	mul_adr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in)*2);
	int time_live=TTL;
	FILE *fp;
	char buf[BUF_SIZE];

	if(argc!=4){
		printf("Usage : %s <GroupIP><PORT><PORT>\n", argv[0]);
		exit(1);
	}
	
	send_sock=socket(PF_INET, SOCK_DGRAM, 0);
	for(int i=0; i<2; i++) {
	memset(&mul_adr[i], 0, sizeof(mul_adr[i]));
	mul_adr[i].sin_family = AF_INET;
	mul_adr[i].sin_addr.s_addr = inet_addr(argv[1]); // Multicast IP
	mul_adr[i].sin_port = htons(atoi(argv[i+2])); // Multicast Port
	}
	
	setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));

	if((fp=fopen("news.txt", "r"))==NULL)
		error_handling("fopen() error");

	while(!feof(fp)) { /* Broadcasting */
		fgets(buf, BUF_SIZE, fp);
		for(int i=0; i<2; i++) {	
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_adr[i], sizeof(mul_adr[i]));
		}
		sleep(2);
	}
	fclose(fp);
	close(send_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
