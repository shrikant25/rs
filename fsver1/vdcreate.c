#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

int set_blkflgs(int, int);


int set_blkflgs(int fd, int flgscnt){
   printf("setting flags and flgscnt %d\n", flgscnt);
   int i = 0;
   unsigned long int flgval = 0xFFFFFFFFFFFFFFFF; 

   //printf("flagval is :  %lx", flgval);
   for(; i < flgscnt/(sizeof(flgval) * 8); i++){
    // printf("writing flag value %lu\n", flgval);
     if( write(fd, &flgval, sizeof(long int)) != sizeof(long int))
      return -1;
   }
   printf("wrote %d blocks\n", i);
   return 0;
}


int main(int argc, char *argv[]){
   
   
   if(argc != 4){
      write(1, "Invalid arguments\n", 18);
      exit(EXIT_FAILURE);
   }
   

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

   /*
   if(blocksize > disksize){
      write(1, "Invalid blocksize/disksize\n", 27);
      exit(EXIT_FAILURE);
   }*/

   blkcnt = dsksz/blksz;
   flgblkcnt = (blkcnt/blksz)/8;
   printf("flag blocks count : %d\n", flgblkcnt);

   fd = open(argv[1], O_CREAT| O_WRONLY, 00777);
   
  
   if(fd == -1){
      write(1, "Disk creation failed\n", 21);
      exit(EXIT_FAILURE);
   }
   
  
   lseek(fd, dsksz-4, SEEK_SET);
   val = 0;
   write(fd, &val, sizeof(val));

   lseek(fd, 0, SEEK_SET);
 
   chptr = (char *)&dsksz_exp; 
   write(fd, chptr, 1);
    
   chptr = (char *)&blksz_exp;
   write(fd, chptr, 1); 
  
   flgsts = set_blkflgs(fd, blkcnt - flgblkcnt); 
	   //first block will be used to store other metadata

   if(flgsts){
   	write(1, "flag setting failed\n", 20);
	exit(EXIT_FAILURE);
   }

   write(1, "Disk creation successfull\n", 26);
 
   close(fd);
   
   return 0;
}
