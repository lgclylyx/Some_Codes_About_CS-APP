#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>

int main(int argc,char* argv[]){
	struct in_addr inaddr;
	unsigned int addr;
	inet_aton(argv[1],&inaddr);
	printf("%x\n",ntohl(inaddr.s_addr));
	return 0;
}
