#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
  
   int fd = open(argv[1], O_RDONLY);
   char buf[1024];
   lseek(fd, atoi(argv[2])-1, SEEK_SET);
   read(fd, buf, atoi(argv[3]));
 	for(int i =0; i < atoi(argv[3]); i++)
   printf("%c", buf[i]);
printf("\n");
     	close(fd);


   return 0;
}
