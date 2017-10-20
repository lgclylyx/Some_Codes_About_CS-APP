#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

int main(int argc,char* argv[]){
	struct in_addr inaddr;
	struct hostent *hostp;
	char** pp;
	if(argc!=2)
		return 1;
	if(inet_aton(argv[1],&inaddr)!=0){
		hostp=gethostbyaddr((const char*)&inaddr,sizeof(inaddr),AF_INET);
	}else
		hostp=gethostbyname(argv[1]);
	printf("official hostname:%s\n",hostp->h_name);
	for(pp=hostp->h_aliases;*pp!=NULL;pp++)
		printf("alias:%s\n",*pp);
	for(pp=hostp->h_addr_list;*pp!=NULL;pp++){
		inaddr.s_addr=((struct in_addr*)*pp)->s_addr;
		printf("address:%s\n",inet_ntoa(inaddr));
	}
	return 0;
}
