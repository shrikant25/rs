#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<string.h>


int createfile(char *filename, unsigned long int dsksz){

   int pid;
   int status;
   char disksize[11];
   sprintf(disksize, "%ld", dsksz);

   pid = fork();
   if(pid == -1){
   	return -1;
   }
   else if(pid > 0){
   	waitpid(pid, &status, 0);
   }
   else{

        char* cargv[5] = {"fallocate", "-l", disksize, filename, NULL};
	execvp(cargv[0], cargv);

	exit(0);
   }
   return 0;	
}
