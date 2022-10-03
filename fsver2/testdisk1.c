#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]){
   int fd = open(argv[1], O_RDONLY);

   char ch;

   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);
  
   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);

   unsigned int val;
   
   read(fd, &val, sizeof(int));
   printf(" val is %u ", val);
   
   unsigned long int *flag; 
   lseek(fd, 1024, SEEK_SET);
   char buffer[1024];
   for(int i = 0; i<514; i++){
   		read(fd, buffer, 1024);
   			flag = (unsigned long int *	)buffer;
   		for(int j = 0; j<1024/8; j++){
   			printf("\nflag is %lx\n", *flag++);
   		}
   		memset(buffer, '\0', 1024);
   }
   
}
