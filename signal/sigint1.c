#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig){
	printf("\ncaught sig_int.\n");
	exit(0);
}

int main(){
	if(signal(SIGINT,handler)==SIG_ERR)
		;
	pause();
	exit(0);
}
