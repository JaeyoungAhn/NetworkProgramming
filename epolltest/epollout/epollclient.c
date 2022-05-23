#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock, suc;
	char buf[BUF_SIZE];
	char message[BUF_SIZE];
	struct sockaddr_in serv_adr;
	FILE *readfp, *writefp, *savefile;
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	
	readfp = fdopen(sock, "r");
	writefp = fdopen(dup(sock), "w");
	fgets(buf, sizeof(buf), readfp);
	buf[strlen(buf)-1]=0;
	printf("%s", buf);
	fgets(message, BUF_SIZE, stdin);
	if(!strcmp(message,"q\n") || !strcmp(message, "Q\n")) { // want to quit
		fputs("Q",writefp);
		fflush(writefp);
		printf("quit the program.\n");
	}
	else { // send file name
		fputs(message,writefp);
		fflush(writefp);
		memset(&buf, 0, sizeof(buf));
		fgets(buf, sizeof(buf), readfp);
		if(!strcmp(buf,"N\n")) { // no such file name
			memset(&buf, 0, sizeof(buf));
			fgets(buf,sizeof(buf),readfp);
			printf("%s", buf);
			fputs("N", writefp);
			fflush(writefp);
			error_handling("terminated the program.");
		}
		else if(!strcmp(buf,"S\n")) { // save the file
			memset(&buf, 0, sizeof(buf));
			message[strlen(message)-1]=0;
			savefile = fopen(message,"wb");
		//	fputs(buf, savefile);
			while(1){
				suc = fread(buf, 1, sizeof(buf), readfp);
				printf("%d\n", suc);
				if(suc==0) break;
				fwrite(buf, 1, suc, savefile);
			}
			fputs("F",writefp);
			fflush(writefp);
			printf("file has been downloaded successfully.\n");
			fclose(savefile);
		}
	}
	fclose(readfp);
	fclose(writefp);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
