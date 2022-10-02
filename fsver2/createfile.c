#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>


int createfile(char *disksize, char *filename){

   int pid;
   int status;
   
   pid = fork();
   if(pid == -1){
   	return -1;
   }
   else if(pid > 0){
   	waitpid(pid, &status, 0);
   }
   else{

        char* cargv[5] = {"fallocate", "-l", strcat(disksize, "K"), filename, NULL};
	execvp(cargv[0], cargv);

	exit(0);
   }
   return 0;	
}
