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
       
   printf("blksz %ld\n", DSKINF.blksz);
   printf("blcknt %ld\n", DSKINF.blkcnt);
   printf("flags arrasz %d\n", DSKINF.flags_arrsz);
   printf("flagsblkcnt %d\n", DSKINF.flgblkcnt);
   printf("ttlmtdta_blks %d\n", DSKINF.ttlmtdta_blks);
   printf(" DSKINF.dsk_blk_for_mtdata %d\n",  DSKINF.dsk_blk_for_mtdata);
   printf(" DSKINF.mtdta_blk_ofst %d\n",  DSKINF.mtdta_blk_ofst);
   printf("flag blocks count : %d\n", DSKINF.flgblkcnt);
   printf("total metadata blocks  %u\n", DSKINF.ttlmtdta_blks);
   printf("dsk blocks required for metadata blocks %d\n", DSKINF.ttlmtdta_blks);

    unsigned long int *flags = malloc(sizeof(unsigned long int)  * (DSKINF.flags_arrsz));
    read(fd, flags, sizeof(unsigned long int) *(DSKINF.flags_arrsz));
    
    for(int i = 0; i < DSKINF.flags_arrsz;i++){
        printf(" %d %lx\n", i , flags[i]);
    }
    free(flags);
    FL_METADATA *mtdarr = malloc(DSKINF.ttlmtdta_blks *sizeof(FL_METADATA));
    read(fd, mtdarr, sizeof(FL_METADATA) * DSKINF.ttlmtdta_blks);
/*
    for(int j = 0; j<DSKINF.ttlmtdta_blks; j++){
        printf("filename : %s\n", mtdarr[j].flnm);
        printf("strloc : %d\n", mtdarr[j].strtloc);
        printf("filesize : %d\n", mtdarr[j].flsz);
        printf("isavailable : %d\n", mtdarr[j].isavailable);
    }*/      
 return 0;
}
