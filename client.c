#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char* message);

int main(int argc, char* argv[]){
	int clnt_sock;
	int str_len;
	struct sockaddr_in serv_adr;
	socklen_t clnt_adr_sz;
	char hostname[BUF_SIZE];
	if(argc !=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(clnt_sock == -1) error_handling("socket() error!");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else puts("connected......");

	while(1){
		fputs("Enter a host name(q to quit): ", stdout);
		fgets(hostname, BUF_SIZE, stdin);
		if(!strcmp(hostname,"q\n")) break;
		write(clnt_sock, hostname, strlen(hostname));
		str_len = read(clnt_sock, hostname, BUF_SIZE-1);
		hostname[str_len]='\0';
		if(hostname[0]==-1) printf("gethost...error\n");
		else printf("%s\n", hostname);
	}
	close(clnt_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
