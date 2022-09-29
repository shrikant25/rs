#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vdfuncs.h"

int main(int argc, char *argv[]){

	char * filename = argv[1];
	unsigned int blocknumber = atoi(argv[2]);
	unsigned int block_size = atoi(argv[3]);
	char buffer[block_size];
	int rdcnt;
	int sz = 280;
	int fd = open(filename, O_RDONLY, 00777);
	if(fd){	
		do{
			rdcnt = vdread(fd, buffer, blocknumber, block_size);
			printf("amount of data read %d\n", rdcnt);
			if(rdcnt > 0)
				printf("data :\n %s", buffer);
			sz--;
			memset(buffer, 0, 1024);
			blocknumber++;
		}while(sz > 0);
		
	}
	return 0;
}
