#include "RIO_USE.h"
#include "my_Socket_use.h"
#include "sbuf_use.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define NTHREADS 4
#define SBUFSIZE 16

sbuf_t sbuf;

extern void echo(int connfd);

void* thread(void* vargp){
	pthread_detach(pthread_self());
	while(1){
		int connfd=sbuf_remove(&sbuf);
		echo(connfd);
		close(connfd);
	}
}

int main(int argc,char** argv){
	int listenfd,connfd,port,clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	pthread_t tid;
	if(argc!=2){
		fprintf(stderr,"usage:%s <port>\n",argv[0]);
		return 0;
	}
	port=atoi(argv[1]);
	sbuf_init(&sbuf,NTHREADS);
	listenfd=open_listenfd(port);

	for(int i=0;i<NTHREADS;i++)
		pthread_create(&tid,NULL,thread,NULL);

	while(1){
		clientlen=sizeof(clientaddr);
		connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
		hp=gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr,sizeof(clientaddr.sin_addr.s_addr),AF_INET);
		haddrp=inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n",hp->h_name,haddrp);
		sbuf_insert(&sbuf,connfd);
	}
	return 0;
}
