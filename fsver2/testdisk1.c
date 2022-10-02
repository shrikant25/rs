#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
   int fd = open(argv[1], O_RDONLY);

   char ch;

   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);
  
   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);

   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);
 
   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);

   unsigned int val;
   lseek(fd, -4, SEEK_END);
   read(fd, &val, sizeof(int));
   printf(" val is %u ", val);
}
