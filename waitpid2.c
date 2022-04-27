#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int status;
	int val;
	pid_t pid = fork();
	if(pid == 0){
		sleep(15);
		return 24;
	}
	else{
		while(val!=0&&(val=waitpid(-1, &status, WNOHANG))){
			printf("%d", val);
			sleep(3);
			puts("Sleep 3 sec.");
		}
		if(WIFEXITED(status))
			printf("Child send %d \n", WEXITSTATUS(status));
	}
	return 0;
}
