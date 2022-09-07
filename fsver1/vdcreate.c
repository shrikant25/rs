#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
   
   
   if(argc != 4){
      write(1, "Invalid arguments\n", 18);
      exit(EXIT_FAILURE);
   }
   

   char ch;
   unsigned char *chptr;
   int fd = 0;
   int i = 0;
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
   
  
   lseek(fd, dsksz-1, SEEK_SET);
   ch = '~';
   write(fd, &ch, sizeof(ch));

   lseek(fd, 0, SEEK_SET);
 
   chptr = (char *)&dsksz_exp; 
   write(fd, chptr, 1);
    
   chptr = (char *)&blksz_exp;
   write(fd, chptr, 1); 

   chptr = (char *)&flgblkcnt;
   write(fd, chptr, 1);
   write(fd, ++chptr, 1);

   write(1, "Disk creation successfull\n", 26);
 
   close(fd);
   
   return 0;
}
