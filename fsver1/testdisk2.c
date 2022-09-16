#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFLEN 1024

typedef struct FL_METADATA{
	char flnm[64];
	unsigned int strtloc;
	unsigned int flsz;
}FL_METADATA;


int main(int argc, char *argv[]){
  
   int fd = open(argv[1], O_RDONLY);
   int fd1;
   char buf[BUFLEN];
   unsigned long flags[65536];
   char dsksz, blksz;
   int data_to_read = 0;
   
   read(fd, &dsksz, sizeof(dsksz));
   read(fd, &blksz, sizeof(blksz));
   read(fd, flags, sizeof(flags));
   
   printf("filesize  %d\n", dsksz);
   printf("blocksize  %d\n", blksz);	
   
   for(int i =0; i<65528; i++){
   	 printf("flag[%d] %lx\n", i, flags[i]);
   }
   
   
  /* lseek(fd, atoi(argv[2])-1, SEEK_SET);
   read(fd, buf, atoi(argv[3]));
 	for(int i =0; i < atoi(argv[3]); i++)
   printf("%c", buf[i]);
printf("\n");
*/

  lseek(fd, -5, SEEK_END);
  unsigned int ttlmetadatablks;
  read(fd, &ttlmetadatablks, sizeof(ttlmetadatablks));
  FL_METADATA flmtd;
	
	printf("ttlmetadatablks %u\n", ttlmetadatablks);
	
  for(int i=1; i<=ttlmetadatablks; i++){
 		lseek(fd, -(i*sizeof(flmtd)+5), SEEK_END);
 		read(fd, &flmtd, sizeof(flmtd));
 		printf("i = %d\n", i);
 		printf("filename %s\n", flmtd.flnm); 
 		printf("start loc %u\n", flmtd.strtloc);
 		printf("filesz %u\n", flmtd.flsz);
 		printf("\n\n");
  }	
  
  char newname[100] = "";

  strcat(newname, "new_");
strcat(newname, flmtd.flnm);
  printf("new name %s", newname);
  
  fd1 = open(newname , O_WRONLY | O_CREAT, 00777);
  if(fd1){
  lseek(fd, flmtd.strtloc-1, SEEK_SET); 
  data_to_read = flmtd.flsz;
  
  while(data_to_read > 0){
  	read(fd, buf, sizeof(char)*BUFLEN);
  	write(fd1, buf, sizeof(char)*BUFLEN);
  	data_to_read -= BUFLEN;
  }
}else{
	printf("failed to display");
}

  close(fd1);
  close(fd);

  return 0;
}
