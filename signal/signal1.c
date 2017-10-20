#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#define MAXBUF 1024	

void handler1(int sig){
	pid_t pid;
	if((pid=waitpid(-1,NULL,0))<0){
		fprintf(stderr,"%s","waitpid error.\n");
		exit(0);
	}
	printf("Handler reaped child %u\n",(int)pid);
	sleep(2);
	return ;
}

int main(){
	int i,n;
	char buf[MAXBUF];
	if(signal(SIGCHLD,handler1)==SIG_ERR){
		fprintf(stderr,"%s","signal error.\n");
		exit(0);
	}
	for(i=0;i<3;i++){
		if(fork()==0){
			printf("hello from child %u\n",getpid());
			sleep(1);
			exit(0);
		}
	}
	if((n=read(STDIN_FILENO,buf,sizeof(buf)))<0){
		fprintf(stderr,"%s","read\n");
		exit(0);
	}
	printf("Parent processing input\n");
	while(1)
		;
	exit(0);
}
