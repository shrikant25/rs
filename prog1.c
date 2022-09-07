#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>

#define bufsize 1024

int main(int argc, char *argv[]){
   int strln = 0;
   int strcnt = 0;
   int idx = 0;
   char ch;
   char buf[bufsize];

   int fd = open(argv[1], O_RDONLY);
   int fd2 = open(argv[2], O_CREAT | O_WRONLY, 0777);

   lseek(fd2, 4, SEEK_SET);

   while(read(fd, &ch, sizeof(char))){
       if(ch !='\n')
          buf[idx++] = ch;
       else{
	  strcnt++;
          write(fd2, &idx, sizeof(idx));
	  printf("ln : %d", idx);
          write(fd2, buf, sizeof(char) * idx);
	  buf[idx] = '\0';
	  printf("str %s\n", buf); 
          buf[0] = '\0';
	  idx = 0;
       }
       
   }
   
   lseek(fd2, 0, SEEK_SET);
   write(fd2, &strcnt, sizeof(strcnt));
    
   close(fd);
   close(fd2);
   return 0;
}
