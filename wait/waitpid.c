#include <sys/types.h>
#include <sys/unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#define N 10
int main(){
	int status,i;
	pid_t pid;
	for(int i=0;i<N;i++)
		if((pid = fork())==0)
			exit(100+i);
	while((pid=waitpid(-1,&status,0))>0){
		if(WIFEXITED(status))
			printf("child %d terminated normally with exit status=%d\n",pid,WEXITSTATUS(status));
		else
			printf("child %d terminated abnormally\n",pid);
	}
	if(errno!=ECHILD)
		fprintf(stderr,"%s",strerror(errno));
	exit(0);
}
