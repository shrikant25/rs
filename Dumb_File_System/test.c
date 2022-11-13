#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv){

   int fd = open(argv[1], O_RDONLY, 00777);
   size_t filesize = lseek(fd, 0, SEEK_END);
   lseek(fd, 0, SEEK_SET);
   char * buffer = malloc(sizeof(char) * filesize);
   memset(buffer, 0, sizeof(char) * filesize);
   read(fd, buffer, sizeof(char) * filesize);
   int cnt = atoi(argv[2]);
   int fd2 = open("task_file", O_CREAT | O_WRONLY, 00777);
   TASK_NODE tskn;
   int task = atoi(argv[3]);
   char temp[52];
    int i =0;

    write(fd2, &cnt, sizeof(int));
   for(int k = 0; k<filesize; k++){
        if(buffer[k] != '\n')
            temp[i++] = buffer[k];
        else{
            temp[i] ='\0';
            i = 0;
            strcpy(tskn.filename, temp);
            if(task == 1)
                strcpy(tskn.path, "/home/shrikant/rs/Dumb_File_System/test_files/");
            else
                strcpy(tskn.path, "/home/shrikant/rs/Dumb_File_System/test_files2/");
            strcat(tskn.path, tskn.filename);
            if(task == 1)
                strcpy(tskn.task, "task_insert_file");
            else if(task == 2)
                strcpy(tskn.task, "task_fetch_file");
            else
                strcpy(tskn.task, "task_delete_file");
            write(fd2, &tskn, sizeof(TASK_NODE));
            temp[0]= '\0';
        } 
   }
  
    close(fd);
   	               close(fd2);
    return 0;
}
