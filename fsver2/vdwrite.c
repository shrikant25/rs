#include "vdsyslib.h"
#include<stdio.h>
   
int vdwrite(int fd, char *buffer, int block_number, int block_size){
	if(block_number == 0)
		printf("\n\n\nwarning________________________________________");
	unsigned int vdbyte_num = block_number * block_size;
	unsigned int cur=	lseek(fd, vdbyte_num, SEEK_SET);
//printf(" cur : %u \n", cur);
//printf(" buffer  %d\n", buffer[block_size-4]);
	unsigned int wrcnt = write(fd, buffer, sizeof(char) * block_size);
	return wrcnt;

}
