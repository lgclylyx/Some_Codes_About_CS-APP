#include "RIO_USE.h"
#include "my_Socket_use.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc,char* argv[]){
	int clientfd,port;
	char *host,buf[1024];
	rio_t rio;
	if(argc!=3){
		fprintf(stderr,"usage:%s <host> <port>\n",argv[0]);
		return 0;
	}
	host=argv[1];
	port=atoi(argv[2]);
	clientfd=open_clientfd(host,port);
	rio_readinitb(&rio,clientfd);
	while(fgets(buf,1024,stdin)!=NULL){
		rio_writen(clientfd,buf,strlen(buf));
		rio_readlineb(&rio,buf,1024);
		fputs(buf,stdout);
	}
	close(clientfd);
	return 0;
}
