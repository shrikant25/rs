#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h> 
   
int vdread(int fd, char *buffer, int block_number, int block_size){
		
	unsigned int vdbyte_num = block_number * block_size;
	lseek(fd, vdbyte_num, SEEK_SET);
	unsigned int rdcnt = read(fd, buffer, sizeof(char) * block_size);
	return rdcnt;

}
