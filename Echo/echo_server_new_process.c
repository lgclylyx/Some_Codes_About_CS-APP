#include "RIO_USE.h"
#include "my_Socket_use.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>

void sigchl_handler(int sig){
	while(waitpid(-1,0,WNOHANG)>0)
		;
	return;
}


void echo(int connfd){
	size_t n;
	char buf[1024];
	rio_t rio;

	rio_readinitb(&rio,connfd);
	while((n=rio_readlineb(&rio,buf,1024))!=0){
		printf("server received %u bytes\n",n);
		rio_writen(connfd,buf,n);
	}
}

int main(int argc,char** argv){
	int listenfd,connfd,port,clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;
	if(argc!=2){
		return 0;
	}
	port=atoi(argv[1]);
	signal(SIGCHLD,sigchl_handler);
	listenfd=open_listenfd(port);
	while(1){
		clientlen=sizeof(clientaddr);
		connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
		hp=gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr,sizeof(clientaddr.sin_addr.s_addr),AF_INET);
		haddrp=inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n",hp->h_name,haddrp);
		if(fork()==0){
			close(listenfd);
			echo(connfd);
			close(connfd);
			return 0;
		}
		close(connfd);
	}
	return 0;
}
