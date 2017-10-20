#include <semaphore.h>
#include <pthread.h>
#include "RIO_USE.h"
#include <stdio.h>

static int byte_cnt;
static sem_t mutex;

static void init_echo_cnt(){
	sem_init(&mutex,0,1);
	byte_cnt=0;
}

void echo(int connfd){
	int n;
	char buf[1024];
	rio_t rio;
	static pthread_once_t once = PTHREAD_ONCE_INIT;

	pthread_once(&once,init_echo_cnt);

	rio_readinitb(&rio,connfd);
	while((n=rio_readlineb(&rio,buf,1024))!=0){
		sem_wait(&mutex);
		byte_cnt+=n;
		printf("thread %d received %d (%d total) bytes on fd %d\n",(int)pthread_self(),n,byte_cnt,connfd);
		sem_post(&mutex);
		rio_writen(connfd,buf,n);
	}
}