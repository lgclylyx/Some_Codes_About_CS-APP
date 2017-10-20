#include <stdio.h>
#include "RIO_USE.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main() {
	rio_t rp;
	int fd;
	//char p[] = "hello,world!";
	char str[2];
	if ((fd = open("test.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
		fprintf(stderr, "open file fail.\n");
	rio_readinitb(&rp,fd);
	//rio_writen(fd, p, sizeof(p));
	while (rio_readlineb(&rp, str, sizeof(str)) > 0)
		printf("%s", str);
	return 0;
}
