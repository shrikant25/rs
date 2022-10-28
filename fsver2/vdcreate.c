#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vdfile_metadata.h"
#include "vdsyslib.h"
#include "vdconstants.h"
#include "vdsetbits.h"
#include "vddriver.h"
#include "vdwrite_to_buffer.h"
#include "vdwrite_flags_to_disk.h"
#include "vddiskinfo.h"

#define METADATA_BLOCKS_RATIO 1/(VDQUAD * VDBYTESZ * VDKB)

int createfile(DISKINFO);
int create_metadata_blocks(int, DISKINFO);

int createfile(DISKINFO DSKINF){

   int pid;
   int status;
   char disksize[11];
   sprintf(disksize, "%ld", DSKINF.dsksz);
   
   pid = fork();
   if(pid == -1){
   	return -1;
   }
   else if(pid > 0){
   	waitpid(pid, &status, 0);
   }
   else{
     
      char* cargv[5] = {"fallocate", "-l", disksize, DSKINF.diskname, NULL};
	   execvp(cargv[0], cargv);

	exit(0);
   }
    
   return 0;	
}


int create_metadata_blocks(int fd, DISKINFO DSKINF){
   
   unsigned int i;
   unsigned int start = 0;
   char *new_buffer = malloc(sizeof(char) * DSKINF.blksz);
   unsigned int ttl_mtdblks_in_dskblk = sizeof(FL_METADATA)/DSKINF.blksz;
   FL_METADATA flmtd;

   strcpy(flmtd.flnm, "");
   flmtd.strtloc = 0;
	flmtd.flsz = 0;
	flmtd.isavailable = 1;

   memset(new_buffer, '\0', DSKINF.blksz);

   for(i = 0; i<ttl_mtdblks_in_dskblk; i++){
      write_to_buffer(new_buffer, (char *)&flmtd, sizeof(FL_METADATA), sizeof(FL_METADATA)*i);
   }
  
   start = DSKINF.mtdta_blk_ofst;
   
   for(i = start; i<DSKINF.dsk_blk_for_mtdata+start ; i++){
      vdwrite(fd, new_buffer, i, DSKINF.blksz);
   }
 
   free(new_buffer);

   return 0;

}


int main(int argc, char *argv[]){
   
   if(argc != 4){
      write(1, "Invalid arguments\n", 18);
      exit(EXIT_FAILURE);
   }
   
   int i, j;
   int fd = 0;
   unsigned char *chptr = NULL;
   unsigned char *buffer;
   unsigned int preoccupied_blocks = 0;
   unsigned int *preoccupied_blocks_list; 
   unsigned long int *flags = NULL;
   DISKINFO DSKINF;

   DSKINF.dsksz = pow(2, atoi(argv[2]));
   DSKINF.blksz = pow(2, atoi(argv[3]));
   buffer = malloc(sizeof(char) * DSKINF.blksz);

   DSKINF.diskname = malloc(sizeof(char) * (strlen(argv[1]) + 1));
   strncpy(DSKINF.diskname, argv[1], strlen(argv[1]));
   DSKINF.diskname[strlen(argv[1])] = '\0'; 

   DSKINF.blkcnt = DSKINF.dsksz/DSKINF.blksz;
   DSKINF.flags_arrsz = DSKINF.blkcnt/(VDQUAD * VDBYTESZ); 
   DSKINF.flgblkcnt = (DSKINF.blkcnt/DSKINF.blksz)/(VDBYTE*VDBYTESZ);
   DSKINF.ttlmtdta_blks = DSKINF.dsksz * METADATA_BLOCKS_RATIO;
   DSKINF.dsk_blk_for_mtdata = (sizeof(FL_METADATA)*DSKINF.ttlmtdta_blks)/DSKINF.blksz;
   DSKINF.mtdta_blk_ofst = DSKINF.flgblkcnt + 1; // after flagblocks 
   
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

   createfile(DSKINF);
   
   fd = open(DSKINF.diskname, O_WRONLY, 00777);

   if(fd == -1){
      write(1, "Disk creation failed\n", 21);
      exit(EXIT_FAILURE);
   }
  
   memset(buffer, 0, DSKINF.blksz);
   write_to_buffer(buffer, (char *)&DSKINF, sizeof(DSKINF), 0);
   vdwrite(fd, buffer, 0, DSKINF.blksz);
   free(DSKINF.diskname);
   free(buffer);

   
   flags = malloc(sizeof(unsigned long int) * DSKINF.flags_arrsz);
   memset(flags, 0xFF, DSKINF.flags_arrsz*VDQUAD);

   preoccupied_blocks = 1 + DSKINF.flgblkcnt + DSKINF.dsk_blk_for_mtdata;
   preoccupied_blocks_list = malloc(sizeof(unsigned int) * preoccupied_blocks);
 
   for(i = 0; i<preoccupied_blocks; i++){
      preoccupied_blocks_list[i] = i;
   }


   setbits(preoccupied_blocks_list, preoccupied_blocks, 0, flags);
   free(preoccupied_blocks_list);
 
   write_flags_todisk(flags, DSKINF);;
   free(flags);
   
   create_metadata_blocks(fd, DSKINF);
   

   write(1, "Disk creation successfull\n", 26);
 
   close(fd);
   

   return 0;
}