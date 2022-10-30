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
   char buf[bufsize];
   int i = 0;
   int idxpos = 0;
   int strpos = 0;

   int fd = open(argv[1], O_RDONLY);
   int fd2 = open(argv[2], O_CREAT | O_WRONLY, 0777);

   read(fd, &strcnt, sizeof(strcnt));
   printf("%d",strcnt);
   write(fd2, &strcnt, sizeof(strcnt));
   strpos = (sizeof(int) * (strcnt + 1));

   for(i=0; i<strcnt; i++){

       read(fd, &strln, sizeof(strln));
       printf("strln %d :", strln);
       write(fd2, &strln, sizeof(strln));
       
       idxpos = lseek(fd, 0, SEEK_CUR);
       lseek(fd, strpos, SEEK_SET);
       
       read(fd, buf, sizeof(char) * strln);
       write(fd2, buf, sizeof(char) * strln);
       buf[strln] ='\0';
       printf("str is : %s\n", buf);
       lseek(fd, idxpos, SEEK_SET);
       strpos += strln;
       buf[0] = '\0';
       strln = 0;  
   }

   close(fd);
   close(fd2);
   return 0;
}
