#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>


const char * szlog_fl = "szlog_fl"; 							// file to check if some other processes have locked all the resources(isbusy_flag)

int main(void){ 

	int fd; 
	unsigned long int flsz = 0;
	
	fd = open(szlog_fl, O_RDONLY);
	if(fd){ 
	
		read(fd, &flsz, sizeof(unsigned long int));
		printf("file size is %ld\n", flsz);
		
		close(fd);
	} 	
	
		 		
	
	return 0;
}
