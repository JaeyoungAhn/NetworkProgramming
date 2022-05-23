#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024
#define EPOLL_SIZE 50
void error_handling(char *buf);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	FILE *readfp[100], *writefp[100];
	FILE *openfile[100]={0,};
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i, index, opencheck, suc;
	char buf[BUF_SIZE];
	char sendbuf[BUF_SIZE];
	char location[BUF_SIZE];

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	printf("serv_sock:%d\n",serv_sock);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events=EPOLLIN;
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
	printf("printf:%d\n", openfile[3]);
	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd==serv_sock) // change from serv_sock
			{
				printf("in serv change\n");
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				printf("clnt:%d\n", clnt_sock);
				event.events=EPOLLIN|EPOLLOUT;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				readfp[clnt_sock]=fdopen(clnt_sock, "r");
				writefp[clnt_sock]=fdopen(dup(clnt_sock), "w");
				printf("rightafter:%d\n", openfile[clnt_sock]);
				printf("client %d has been connected.\n", (clnt_sock-3)/2);
				fputs("Enter name of the file you want to download including the file extension(input q to quit):\n", writefp[clnt_sock]);
				fflush(writefp[clnt_sock]);
				printf("dump\n");
				printf("clnt_sock:%d\n", clnt_sock);

			}
			else
			{ // change from clnt_sock	
				printf("before index\n");
				index = ep_events[i].data.fd;
				printf("index:%d\n", index);
				printf("before index2\n");

	//			opencheck = ftell(openfile[index]);
				printf("before index3\n");
	//			printf("opencheck:%d\n", opencheck);
				if(ep_events[i].events & EPOLLIN) { // receive from client
					printf("after EPOLLIN\n");
					fgets(buf, BUF_SIZE, readfp[index]);// receive a line from client
					printf("after epollin2\n");
					if(strlen(buf)==1) { // receive a signal from client
						printf("hellofinish\n");
						epoll_ctl(epfd, EPOLL_CTL_DEL, index, NULL);
						printf("hellofinish2\n");
						fclose(readfp[index]);
						printf("hellofinish3\n");
						fclose(writefp[index]);
						printf("hellofinish4\n");
						printf("closedfinish\n");
						if(buf[0]=='q' || buf[0]=='Q') {
							printf("client %d quit without downloading.\n", (index-3)/2);	
						}
						else if(buf[0]=='F') {
							printf("client %d finished downloading.\n", (index-3)/2);
						}
						else if(buf[0]=='N') {
							printf("terminated client %d because input file does not exist.\n", (index-3)/2);
						}
						printf("closedfinish2\n");
					}
					else {
						memset(&location, 0, sizeof(location));
						strcat(location, "./");
						strcat(location,buf);
						location[strlen(location)-1]=0;
						if(access(location, F_OK) == 0 ) { // file exist
							fputs("S\n", writefp[index]);
							fflush(writefp[index]);
							openfile[index]=(FILE*)malloc(sizeof(FILE));
							openfile[index]=fopen(location, "rb");
						} else { // file does not exist
							fputs("N\n", writefp[index]);
							fflush(writefp[index]);
							fputs("File does not exist.\n", writefp[index]);
							fflush(writefp[index]);
						}
					}
				}
				else if((ep_events[i].events & EPOLLOUT) && openfile[index]!=NULL ) { // send to client
					printf("after EPOLLOUT\n");
					suc = fread(sendbuf, 1, sizeof(sendbuf), openfile[index]);
					printf("%d\n", suc);
					if(feof(openfile[index])!=0){
						fwrite(sendbuf, 1, suc, writefp[index]);
						fflush(writefp[index]);
						shutdown(fileno(writefp[index]), SHUT_WR);
						fclose(openfile[index]);
						openfile[index]=0;
					}
					else {
						fwrite(sendbuf, 1, suc, writefp[index]);
					//	fflush(writefp[index]);
					}
					printf("after ePOLLOUT2\n");
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void error_handling(char* buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}
