#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#define BUF_SIZE 1024

void error_handling(char* message);

int main(int argc, char* argv[]){
	int serv_sock, clnt_sock, i, str_len;
	struct sockaddr_in serv_adr, clnt_adr;
	struct hostent* host;
	socklen_t clnt_adr_sz;
	char hostname[BUF_SIZE];
	char hostadr[BUF_SIZE];
	
	if(argc !=2){
		printf("Usage: %s <port>\n", argv[0]);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1) error_handling("socket() error!");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))) error_handling("bind() error!");
	if(listen(serv_sock, 5) == -1) error_handling("listen() error!");

	clnt_adr_sz = sizeof(clnt_adr);

	for(i=0; i<5; i++){
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock == -1) error_handling("accept() error!");

		while((str_len = read(clnt_sock, hostname, BUF_SIZE)) !=0){
			hostname[str_len-1]='\0';
	
			host = gethostbyname(hostname);
			if(host){
				strcpy(hostadr, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
				write(clnt_sock, hostadr, strlen(hostadr));
			}
			else {
				hostadr[0]=-1;
				hostadr[1]='\0';
				write(clnt_sock, hostadr, sizeof(char));
			}
		}
		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
