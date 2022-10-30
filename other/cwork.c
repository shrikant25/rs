#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){

  long var[] = {0x12345678aaaaaaaa,
  	        0x12345678bbbbbbbb,
  	        0x12345678cccccccc,
                0x12345678dddddddd,
                0x12345678eeeeeeee,
                0x12345678ffffffff,
                0xabcabcbacbabcabc,
                0xabababababababba, 
                0xcdcdcdcdcdcdcdcd,
                0xefefefefefefefef};

 
  int fd = open("cwork_data", O_CREAT | O_WRONLY);
  for(int i = 0; i<10; i++){
     write(fd, &var[i], 8);
  }
  close(fd);

  fd = open("cwork_data", O_RDONLY);
  
  unsigned char buf[8];

  for(int i = 0; i<10; i++){
     read(fd, buf, 8);
     unsigned char * p = buf;
     for(int j = 0; j<8; j++)
     	printf("%x", *(p+j));
     printf("\n");
  }

  close(fd);

  return 0;
}
