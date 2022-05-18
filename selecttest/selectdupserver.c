#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]) {
	int serv_sock, clnt_sock;
	FILE *readfp[100], *writefp[100], *openfile[100];
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	socklen_t adr_sz;
	int fd_max, fd_num, i;
	char buf[BUF_SIZE] = {0, };
	char sendbuf[BUF_SIZE];
	char location[BUF_SIZE];
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
			if(FD_ISSET(i, &cpy_reads)){ // if there's a change on ith element 
				if(i == serv_sock) { // change from serv_sock
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;
					readfp[clnt_sock]=fdopen(clnt_sock, "r");
					writefp[clnt_sock]=fdopen(dup(clnt_sock), "w");
					printf("client %d has been connected.\n", (clnt_sock-2)/2);
					fputs("Enter name of the file you want to download including the file extension(input q to quit):\n", writefp[clnt_sock]);
					fflush(writefp[clnt_sock]);
				}
				else { // change from clnt_sock
					fgets(buf, sizeof(buf), readfp[i]);// receive a line from client
					if(strlen(buf)==1) { // received signal from client
						FD_CLR(i, &reads);
						fclose(readfp[i]);
						if(buf[0]=='q' || buf[0]=='Q') {
							printf("client %d quit without downloading.\n", (i-2)/2);	
						}
						else if(buf[0]=='F') {
							printf("client %d finished downloading.\n", (i-2)/2);
						}
						else if(buf[0]=='N') {
							printf("terminated client %d because input file does not exist.\n", (i-2)/2);
						}
					}
					else { // client request file to send
						memset(&location, 0, sizeof(location));
						strcat(location,"./");
						strcat(location,buf);
						location[strlen(location)-1]=0;
						if(access(location, F_OK) == 0 ) { // file exist
							fputs("S\n", writefp[i]);
							fflush(writefp[i]);
							openfile[i]=malloc(sizeof(FILE*));
							openfile[i]=fopen(location,"r");
							while(1){
								if(fgets(sendbuf, sizeof(sendbuf), openfile[i]) == NULL) break;
								fputs(sendbuf, writefp[i]);
								fflush(writefp[i]);
							}
							fclose(openfile[i]);
						} else { // file does not exist
							fputs("N\n", writefp[i]);
							fflush(writefp[i]);
							fputs("File does not exist.\n", writefp[i]);
							fflush(writefp[i]);
						}
						shutdown(fileno(writefp[i]), SHUT_WR);
						fclose(writefp[i]);
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
