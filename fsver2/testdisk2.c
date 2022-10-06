#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#define BUFLEN 1024

typedef struct FL_METADATA{
	char flnm[64-12];
    unsigned int isavailable;
	unsigned int strtloc;

	unsigned int flsz;
}FL_METADATA;

int vdread(int, char*, int, int);
int main(int argc, char *argv[]){
  
   int fd = open(argv[1], O_RDONLY);
   int fd1;
   char buf[BUFLEN];
   unsigned long flags[65536];
   char dsksz, blksz;
   int data_to_read = 0;
   
   read(fd, &dsksz, sizeof(char));
   read(fd, &blksz, sizeof(char));
   
  unsigned int ttlmetadatablks = 0;
  read(fd, &ttlmetadatablks, sizeof(int));
   printf("ttlmetadatablks %u\n", ttlmetadatablks);
 
 lseek(fd, 1024, SEEK_SET);

   read(fd, flags, sizeof(flags));
  printf("filesize  %d\n", dsksz);
   printf("blocksize  %d\n", blksz);	
   
   /*for(int i = 0; i<65536; i++){
   	 printf("flag[%d] %lx\n", i, flags[i]);
   }*/
   
   
  /* lseek(fd, atoi(argv[2])-1, SEEK_SET);
   read(fd, buf, atoi(argv[3]));
 	for(int i =0; i < atoi(argv[3]); i++)
   printf("%c", buf[i]);
printf("\n");
*/

  unsigned int curblock = (pow(2,dsksz))/(pow(2,blksz));
  FL_METADATA *flmtd;
  printf("cur block %d is \n", curblock);
	char *buf2 = malloc(sizeof(char) * 1024);
 int i = 1;
 while(i<=ttlmetadatablks){
   vdread(fd, buf2, curblock-i,1024);
  
   int mtdblks = (pow(2,blksz))/sizeof(FL_METADATA);
   printf("size of metada lovk %ld\n", sizeof(FL_METADATA));
      flmtd = (FL_METADATA *)buf2;

   for(int j = 0; j<mtdblks && i<=ttlmetadatablks; j++){

      if(flmtd->isavailable){


            printf("filename %s\n", flmtd->flnm); 
            printf("is availalble %u\n", flmtd->isavailable);
            printf("start loc %u\n", flmtd->strtloc);
            printf("filesz %u\n", flmtd->flsz);
            printf("\n\n");   


            char newname[100] = "";
            strcat(newname, "new_");
           strcat(newname, flmtd->flnm);
            printf("new name %s", newname);
        
          fd1 = open(newname , O_WRONLY | O_CREAT, 00777);
            if(fd1){
              lseek(fd, flmtd->strtloc*1024, SEEK_SET); 
              data_to_read = flmtd->flsz;
              printf("datat to read %d\n", data_to_read);
              while(data_to_read > 0){
                  memset(buf, '\0', BUFLEN);
                read(fd, buf, sizeof(char)*BUFLEN);
                write(fd1, buf, sizeof(char)*BUFLEN);
               printf("writing %s \n", buf);
                data_to_read -= BUFLEN;
              }
                close(fd1);
            }else{
          printf("failed to display");
             } 
         }
          flmtd++;
          i++;
      }

 }



  close(fd);

  return 0;
}
