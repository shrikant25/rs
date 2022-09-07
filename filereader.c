#include<stdio.h>

int main(int argc, char *argv[]){
  
  char *filename = argv[1];
  int fd = open(filename, O_RDONLY);
  char ch;

  while(read(fd, ch, 1)
    write(1, &ch, ch);  

  close(fp);
  return 0;
}
