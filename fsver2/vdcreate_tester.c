#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "vdfile_metadata.h"
#include "vddiskinfo.h"
#include <string.h>

DISKINFO DSKINF;

int main(int argc, char *argv[]){
    
    int fd = open(argv[1], O_RDONLY);

    read(fd, &DSKINF, sizeof(DSKINF));
       
    unsigned int bytes_to_read = DSKINF.dsksz/(DSKINF.blksz*64); 
    printf("%ld\n", lseek(fd,  DSKINF.blksz, SEEK_SET));

    unsigned long int *buffer = malloc(bytes_to_read * sizeof(unsigned long int));
    read(fd, buffer, sizeof(unsigned long int)* bytes_to_read);
    
    printf("bytes to read %d\n", bytes_to_read);
    for(int i = 0; i<bytes_to_read; i++){
   		printf("\nflag is %d %lx\n", i, buffer[i]);
    }

    FL_METADATA mtdarr[DSKINF.ttlmtdta_blks];
    read(fd, mtdarr, sizeof(FL_METADATA) * DSKINF.ttlmtdta_blks);

    for(int j = 0; j<DSKINF.ttlmtdta_blks; j++){
        printf("filename : %s\n", mtdarr[j].flnm);
        printf("strloc : %d\n", mtdarr[j].strtloc);
        printf("filesize : %d\n", mtdarr[j].flsz);
        printf("isavailable : %d\n", mtdarr[j].isavailable);
    }      
 return 0;
}
