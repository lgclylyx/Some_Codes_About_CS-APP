#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define MAXRGS 128
#define MAXLINE 100
extern char **environ;
void eval(char*);
int parseline(char*,char**);
int builtin_command(char**);
int main(){
	char cmdline[MAXLINE];
	while(1){
		printf("> ");
		fgets(cmdline,MAXLINE,stdin);
		if(feof(stdin))
			exit(0);
		eval(cmdline);
	}
}
void eval(char* cmdline){
	char *argv[MAXRGS];
	char buf[MAXLINE];
	int bg;
	pid_t pid;
	strcpy(buf,cmdline);
	bg = parseline(buf,argv);
	if(argv[0]==NULL)
		return;
	if(!builtin_command(argv)){
		if((pid=fork())==0){
			if(execve(argv[0],argv,environ)<0){
				printf("%s: Command not found.\n",argv[0]);
				exit(0);
			}
		}
		if(!bg){
			int status;
			if(waitpid(pid,&status,0)<0)
				fprintf(stderr,"%s","waitfg:waitpid error\n");
		}
		else
			printf("%d %s",pid,cmdline);
	}
	return;
}
int builtin_command(char**argv){
	if(!strcmp(argv[0],"quit"))
		exit(0);
	if(!strcmp(argv[0],"&"))
		return 1;
	if(!strcmp(argv[0],""))
		return 1;
	return 0;
}
int parseline(char* buf,char** argv){
	char *delim;
	int argc;
	int bg;
	buf[strlen(buf)-1]==' ';
	while(*buf&&(*buf==' '))
		buf++;
	argc=0;
	while((delim=strchr(buf,' '))){
		argv[argc++]=buf;
		*delim='\0';
		buf=delim+1;
		while(*buf&&(*buf==' '))
			buf++;
	}
	if(*buf=='\0')
		argv[argc]=NULL;
	else{
		buf[strlen(buf)-1]='\0';
		argv[argc++]=buf;
		argv[argc]=NULL;
	}
	if(argc==0)
		return 1;
	if((bg=(*argv[argc-1]=='&'))!=0)
		argv[--argc]=NULL;
	return bg;
}
