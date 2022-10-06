#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "vdheader.h"
#include "vdsyslib.h"



int set_blkflgs(int fd, int flgscnt, char * buffer, unsigned int blksz){
  
   int i;
   int status = 0;
   //printf("flagval is :  %lx", flgval);
   int blocks_to_write = ceil((double)flgscnt/(8*blksz));
   int bytes_to_write = 0;
  
   for(i = 1; i<=blocks_to_write; i++){

		
	if((flgscnt/8) > blksz)
		bytes_to_write = blksz;
	else
		bytes_to_write = flgscnt/8;

   if(i != 1){
      memset(buffer, 0xFF, bytes_to_write);
   }else{
	   memset(buffer, 0x00, 64);
      memset(buffer+64, 0xFE, 1);
      memset(buffer+65, 0xFF, bytes_to_write-65);
   }
  // 	printf("in c buffer is %d\n", buffer[0]);
//	printf("%d written\n", bytes_to_write);
	status = vdwrite(fd, buffer, i, blksz);
	if(status <= 0)
		return -1;
	flgscnt -= (bytes_to_write*8);
//	printf("%d flags\n", flgscnt);
   }

   return 0;
   
}


int main(int argc, char *argv[]){
   
   
   if(argc != 4){
      write(1, "Invalid arguments\n", 18);
      exit(EXIT_FAILURE);
   }
   
   int i;
   unsigned char *chptr;
   int fd = 0;
   unsigned int val = 0;
   short int flgsts = 0;
   unsigned long int dsksz = 0;
   unsigned int dsksz_exp = 0;
   unsigned int blksz = 0;
   unsigned int blksz_exp = 0;
   unsigned int blkcnt = 0;
   unsigned int flgblkcnt = 0;

   dsksz_exp = atoi(argv[2]);
   blksz_exp = atoi(argv[3]);
   dsksz = pow(2, dsksz_exp);
   blksz = pow(2, blksz_exp);
   unsigned char *buffer = calloc((blksz), sizeof(char));

   blkcnt = dsksz/blksz;
   flgblkcnt = (blkcnt/blksz)/8;
   printf("flag blocks count : %d\n", flgblkcnt);


  createfile(argv[1], dsksz);

   fd = open(argv[1], O_WRONLY, 00777);
   
  
   if(fd == -1){
      write(1, "Disk creation failed\n", 21);
      exit(EXIT_FAILURE);
   }
   
   chptr = (char *)&dsksz_exp; 
   buffer[0] = *chptr;
    
   chptr = (char *)&blksz_exp;
   buffer[1] =  *chptr;

   val = 0;	
	chptr = (char *)&val;
	buffer[2] = *chptr++;
	buffer[3] = *chptr++;
	buffer[4] = *chptr++;
	buffer[5] = *chptr;

   vdwrite(fd, buffer, 0, blksz);
  
   flgsts = set_blkflgs(fd, blkcnt, buffer, blksz); 
	   //first block will be used to store other metadata
   if(flgsts){
   	write(1, "flag setting failed\n", 20);
	exit(EXIT_FAILURE);
   }

   write(1, "Disk creation successfull\n", 26);
 
   close(fd);
   
   return 0;
}
