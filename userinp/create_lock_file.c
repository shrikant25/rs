#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>


const char * lck_stsfl = "lck_stsfl"; 							// file to check if some other processes have locked all the resources(isbusy_flag)

int main(int argc, char *argv[]){ 

	unsigned char status = atoi(argv[1]);
	int fd; 
	
	fd = open(lck_stsfl, O_RDWR | O_CREAT, 00777);
	if(fd){ 
	
		printf("writing to file %d\n size %ld\n", status, sizeof(status));
		size_t bytes =	write(fd, &status, sizeof(unsigned char));
		printf("this %ld much bytes written\n", bytes);
	
	
		status = -1;	
		
		lseek(fd, SEEK_SET, 0);
		
		read(fd, &status, sizeof(unsigned char));
		printf("reading from file %d\n", status);
		
		close(fd);
	} 	
	
		 		
	
	return 0;
}
