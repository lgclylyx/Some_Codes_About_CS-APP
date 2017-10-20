#include <signal.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
jmp_buf buf;

void handler(int sig){
	siglongjmp(buf,1);
}
int main(){
	signal(SIGINT,handler);
	if(!sigsetjmp(buf,1))
		printf("starting...\n");
	else
		printf("restarting...\n");
	while(1){
		sleep(1);
		printf("processing...\n");
	}
	exit(0);
}
