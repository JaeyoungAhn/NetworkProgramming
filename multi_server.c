#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;

	pid_t pid;
	struct sigaction act;
	struct hostent* host;
	socklen_t adr_sz;
	int str_len, state;
	char buf[BUF_SIZE];
	char hostadr[BUF_SIZE];
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	state = sigaction(SIGCHLD, &act, 0);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while(1){
		adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
		if(clnt_sock == -1)
			continue;
		else
			puts("new client connected ...");
		pid = fork();
		if(pid == -1){ // parent process
			close(clnt_sock);
			continue;
		}
		if(pid == 0){ // child process
			close(serv_sock);
			while((str_len = read(clnt_sock, buf, BUF_SIZE))!=0){
				buf[str_len-1]=0;
				host = gethostbyname(buf);
				if(host){
					for(int i=0; host->h_addr_list[i]; i++) {
						strcpy(hostadr, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
						hostadr[strlen(hostadr)+1]=0;
						hostadr[strlen(hostadr)]='\n';
						write(clnt_sock, hostadr, strlen(hostadr));
					}
				}
				else {
					hostadr[0]=-1;
					hostadr[1]=0;
					write(clnt_sock, hostadr, sizeof(char));
				}
			}
			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void read_childproc(int sig){
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d\n", pid);
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
