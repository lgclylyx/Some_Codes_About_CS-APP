#include "my_Socket_use.h"
#include "RIO_USE.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define MAXBUF 4096
extern char** environ;

void doit(int fd);
void clienterror(int fd,char* cause,char* errnum,char* shortmsg,char *longmsg);
void read_requesthders(rio_t *rp);
int parse_uri(char* uri,char *filename,char *cgiargs);
void serve_static(int fd,char *filename,int filesize);
void get_filetype(char* filename,char* filetype);
void serve_dynamic(int fd,char* filename,char* cgiargs);



int main(int argc,char** argv){
	int listenfd,connfd,port,clientlen;
	struct sockaddr_in clientaddr;
	if(argc!=2){
		fprintf(stderr,"usage:%s <port>\n",argv[0]);
		return 0;
	}
	port=atoi(argv[1]);
	listenfd=open_listenfd(port);
	while(1){
		clientlen=sizeof(clientaddr);
		connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
		doit(connfd);
		close(connfd);
	}
}

void doit(int fd){
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	char filename[MAXLINE],cgiargs[MAXLINE];
	rio_t rio;

	rio_readinitb(&rio,fd);
	rio_readlineb(&rio,buf,MAXLINE);
	sscanf(buf,"%s %s %s",method,uri,version);
	if(strcasecmp(method,"GET")){
		clienterror(fd,method,"501","Not Implemented","Tiny does not implement this method");
		return;
	}
	read_requesthders(&rio);
	is_static=parse_uri(uri,filename,cgiargs);
	if(stat(filename,&sbuf)<0){
		clienterror(fd,filename,"404","Not found","Tiny couldn't find this file");
		return;
	}
	if(is_static){
		if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR&sbuf.st_mode)){
			clienterror(fd,filename,"403","Forbidden","Tiny couldn't read this file");
			return;
		}
		serve_static(fd,filename,sbuf.st_size);
	}else{
		if(!(S_ISREG(sbuf.st_mode))||!(S_IXUSR&sbuf.st_mode)){
			clienterror(fd,filename,"403","Forbidden","Tiny couldn't run the CGI program");
			return;
		}
		serve_dynamic(fd,filename,cgiargs);
	}
}

void clienterror(int fd,char* cause,char* errnum,char* shortmsg,char *longmsg){
	char buf[MAXLINE],body[MAXBUF];
	sprintf(body,"<html><title>Tiny Error</title>");
	sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
	sprintf(body,"%s%s:%s\r\n",body,errnum,shortmsg);
	sprintf(body,"%s<p>%s: %s\r\n",body,longmsg,cause);
	sprintf(body,"%s<hr><em>The Tiny Web server</em>\r\n",body);
	sprintf(buf,"HTTP/1.0 %s %s\r\n",errnum,shortmsg);
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-type:text/html\r\n");
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-length:%d\r\n\r\n",(int)strlen(body));
	rio_writen(fd,buf,strlen(buf));
	rio_writen(fd,body,strlen(body));
}

void read_requesthders(rio_t *rp){
	char buf[MAXLINE];
	rio_readlineb(rp,buf,MAXLINE);
	printf("%s",buf);
	while(strcmp(buf,"\r\n")){
		rio_readlineb(rp,buf,MAXLINE);
		printf("%s",buf);
	}
	return;
}

int parse_uri(char* uri,char *filename,char *cgiargs){
	char *ptr;
	if(!strstr(uri,"cgi-bin")){
		strcpy(cgiargs,"");
		strcpy(filename,"/home/liuyangxi/workspace/ns3/doc/html");
		strcat(filename,uri);
		if(uri[strlen(uri)-1]=='/')
			strcat(filename,"index.html");
		return 1;
	}else{
		ptr=index(uri,'?');
		if(ptr){
			strcpy(cgiargs,ptr+1);
			*ptr='\0';
		}else{
			cgiargs="";
		}
		strcpy(filename,".");
		strcat(filename,uri);
		return 0;
	}
}

void serve_static(int fd,char *filename,int filesize){
	int srcfd;
	char *srcp,filetype[MAXLINE],buf[MAXBUF];
	get_filetype(filename,filetype);
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	sprintf(buf,"%sServer:Tiny Web Server\r\n",buf);
	sprintf(buf,"%sContent-length:%d\r\n",buf,filesize);
	sprintf(buf,"%sContent-type:%s\r\n\r\n",buf,filetype);
	rio_writen(fd,buf,strlen(buf));
	srcfd=open(filename,O_RDONLY,0);
	srcp=mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
	close(srcfd);
	rio_writen(fd,srcp,filesize);
	munmap(srcp,filesize);
}

void get_filetype(char* filename,char* filetype){
	if(strstr(filename,".html"))
		strcpy(filetype,"text/html");
	else if(strstr(filename,".gif"))
		strcpy(filetype,"image/gif");
	else if(strstr(filename,".jpg"))
		strcpy(filetype,"image/jpeg");
	else
		strcpy(filetype,"text/plain");
}

void serve_dynamic(int fd,char* filename,char* cgiargs){
	char buf[MAXLINE],*emptylist[]={NULL};
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Server: Tiny Web Server\r\n");
	rio_writen(fd,buf,strlen(buf));
	if(fork()==0){
		setenv("QUERY_STRING",cgiargs,1);
		dup2(fd,STDOUT_FILENO);
		execve(filename,emptylist,environ);
	}
	wait(NULL);
}