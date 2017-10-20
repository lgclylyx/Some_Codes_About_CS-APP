#include <arpa/inet.h>
#include <stdio.h>
#include<netinet/in.h>
int main(int argc,char* argv[]){
	unsigned int addr;
	struct in_addr inaddr;
	if(argc!=2){
		fprintf(stderr,"usage:%s <hex number>\n",argv[0]);
		return 0;
	}
	sscanf(argv[1],"%x",&addr);
	inaddr.s_addr=htonl(addr);
	printf("%s",inet_ntoa(inaddr));
	return 0;
}
