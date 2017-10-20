#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
int main(){
	pid_t pid;
	if((pid=fork())==0){
		pause();
		printf("control should never reach here!\n");
		exit(0);
	}
	kill(pid,SIGKILL);
	exit(0);
}
