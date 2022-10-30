#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){

  long var = 0x12345678aaaaaaaa;

  int fd = open("cwork1_data", O_CREAT | O_WRONLY);
  unsigned char * p =(unsigned char *) &var;
  
  for(int i=0; i<8; i++){
     write(fd, p+i, 1);
  }

  close(fd);

  fd = open("cwork1_data", O_RDONLY);
 
  for(int j = 0; j<8; j++){
     read(fd, p, 1);
     printf("%x ", *(p+j));
  }
 

  close(fd);

  return 0;
}
