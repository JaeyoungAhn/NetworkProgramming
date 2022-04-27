#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void error_handling(char *message);

int main(int argc, char *argv[]) {
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	char mes[50];
	char order[2];
	char* mes3="Server : Welcome~\n";
	char clnum=1;
	if(argc !=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while(1){
		cpy_reads = reads;
		if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, NULL)) == -1)
			break;
		if(fd_num == 0)
			continue;
		for(i = 3; i < fd_max + 1; i++){
			if(FD_ISSET(i, &cpy_reads)){
				if(i == serv_sock){
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d \n", clnt_sock);
					write(clnt_sock, mes3, strlen(mes3));
					memset(&mes, 0, 50);
					strcat(mes, "The number of clients is ");
					order[0]=(clnum++)+48;
					order[1]=0;
					strcat(mes, order);
					strcat(mes, " now.\n");
					write(clnt_sock, mes, strlen(mes));
					for(int j=4; j<=fd_max; j++) {
						if(FD_ISSET(j, &reads)){
						memset(&mes, 0, 50);
						strcpy(mes, "server: client ");
						order[0]=(char)clnt_sock+48;
						order[1]=0;
						strcat(mes, order);
						strcat(mes, " has joined this chatting room\n");
						if(j != clnt_sock) write(j, mes, strlen(mes));
						}
					}
				}else {
					str_len = read(i, buf, BUF_SIZE);
					if(str_len == 0){
						FD_CLR(i, &reads);
						close(i);
						clnum--;
						printf("closed client: %d \n", i);
						for(int j=4; j<=fd_max; j++) {
							if(FD_ISSET(j, &reads)){
								memset(&mes, 0, 50);
								strcat(mes, "client ");
								order[0]=(char)i+48;
								order[1]=0;
								strcat(mes, order);
								strcat(mes, " has left this chatting room\n");
								write(j, mes, strlen(mes));			
							}
						}

					}else{
						buf[str_len]=0;	
						for(int j=4; j<=fd_max; j++){
							memset(&mes, 0, 50);
							strcpy(mes, "client ");
							order[0]=(char)i+48;
							order[1]=0;
							strcat(mes, order);
							strcat(mes, " : ");
							strcat(mes, buf);
							if(j != i) write(j, mes, strlen(mes)); 
						}
					}
				}
			}	
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
