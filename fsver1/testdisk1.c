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

   lseek(fd, 0, SEEK_END);
   read(fd, &ch, sizeof(char));
   printf(" %d ", ch);
}
