#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

typedef struct FL_METADATA{
	char flnm[52];
	unsigned int strtloc;
	unsigned int flsz;
	unsigned int isavailable;
}FL_METADATA;

int main(int argc, char *argv[]){
    
    int fd = open(argv[1], O_RDONLY);

    unsigned long int disksize = 0;
    unsigned long int blocksize = 0;
    unsigned int total_metadata_blocks = 0;
    unsigned int metadata_block_offset = 0;

    read(fd, &disksize, sizeof(long int));
    printf(" Disksize : %ld \n", disksize);
  
    read(fd, &blocksize, sizeof(long int));
    printf(" Blockksize : %ld \n", blocksize);

    read(fd, &total_metadata_blocks, sizeof(unsigned int));
    printf(" Total metadata blocks %u ", total_metadata_blocks);
   
    read(fd, &metadata_block_offset, sizeof(int));
    printf(" Metadata blocks offset %u ", metadata_block_offset);
   
    unsigned int bytes_to_read = disksize/(blocksize*64); 
    printf("%ld\n", lseek(fd,  blocksize, SEEK_SET));

    unsigned long int *buffer = malloc(bytes_to_read * sizeof(unsigned long int));
    read(fd, buffer, sizeof(unsigned long int)* bytes_to_read);
    
    printf("bytes to read %d\n", bytes_to_read);
    for(int i = 0; i<bytes_to_read; i++){
   		printf("\nflag is %d %lx\n", i, buffer[i]);
    }

    FL_METADATA mtdarr[total_metadata_blocks];
    read(fd, mtdarr, sizeof(FL_METADATA) * total_metadata_blocks);

    for(int j = 0; j<total_metadata_blocks; j++){
        printf("filename : %s\n", mtdarr[j].flnm);
        printf("strloc : %d\n", mtdarr[j].strtloc);
        printf("filesize : %d\n", mtdarr[j].flsz);
        printf("isavailable : %d\n", mtdarr[j].isavailable);
    }      
 return 0;
}
