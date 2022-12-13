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
int create_metadata_blocks(int, DISKINFO, char *);

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


int create_metadata_blocks(int fd, DISKINFO DSKINF, char *buffer){
   
   unsigned int i;
   unsigned int start = 0;

   unsigned int ttl_mtdblks_in_dskblk = DSKINF.blksz/sizeof(FL_METADATA);
   FL_METADATA flmtd;

   strcpy(flmtd.flnm, "");
   flmtd.strtloc = -1;
   flmtd.flsz = -1;
   flmtd.isavailable = 1;

   memset(buffer, 0, DSKINF.blksz);

   for(i = 0; i<ttl_mtdblks_in_dskblk; i++){
      write_to_buffer(buffer, (char *)&flmtd, sizeof(FL_METADATA), sizeof(FL_METADATA)*i);
   }

   start = DSKINF.mtdta_blk_ofst;
   for(i = start; i<DSKINF.dsk_blk_for_mtdata+start ; i++){
      vdwrite(fd, buffer, i, DSKINF.blksz);
   }

   return 0;
   
}


int main(int argc, char *argv[]){
   
   if(argc != 4){
      printf("Invalid arguments\n");
      exit(EXIT_FAILURE);
   }
   
   int i, j;
   int fd = 0;
   unsigned char *chptr = NULL;
   unsigned char *buffer = NULL;
   unsigned int preoccupied_blocks = 0;
   unsigned int occupblk_to_set = 0;
   DISKINFO DSKINF;

   DSKINF.dsksz = pow(2, atoi(argv[2]));
   DSKINF.blksz = pow(2, atoi(argv[3]));
   buffer = malloc(sizeof(char) * DSKINF.blksz);
   if(buffer == NULL){
      printf("In sufficient memory\n");
      exit(EXIT_FAILURE);
   }

   DSKINF.diskname = malloc(sizeof(char) * (strlen(argv[1]) + 1));
   if(DSKINF.diskname == NULL){
      printf("In sufficient memory\n");
      exit(EXIT_FAILURE);
   }

   strcpy(DSKINF.diskname, argv[1]);
   DSKINF.diskname[strlen(argv[1])] = '\0'; 

   DSKINF.blkcnt = DSKINF.dsksz/DSKINF.blksz;
   DSKINF.flags_arrsz = DSKINF.blkcnt/(VDQUAD * VDBYTESZ); 
   DSKINF.flgblkcnt = (DSKINF.blkcnt/DSKINF.blksz)/(VDBYTE*VDBYTESZ);
   DSKINF.ttlmtdta_blks = DSKINF.dsksz * METADATA_BLOCKS_RATIO;
   DSKINF.dsk_blk_for_mtdata = (sizeof(FL_METADATA)*DSKINF.ttlmtdta_blks)/DSKINF.blksz;
   DSKINF.mtdta_blk_ofst = DSKINF.flgblkcnt + 1; // after flagblocks 
   
   createfile(DSKINF);
   
   fd = open(DSKINF.diskname, O_WRONLY, 00777);

   if(fd == -1){
      printf("Disk creation failed\n");
      exit(EXIT_FAILURE);
   }
  
   memset(buffer, 0, DSKINF.blksz);
   write_to_buffer(buffer, (char *)&DSKINF, sizeof(DSKINF), 0);
   vdwrite(fd, buffer, 0, DSKINF.blksz);
   
   free(DSKINF.diskname);
   
   preoccupied_blocks = 1 + DSKINF.flgblkcnt + DSKINF.dsk_blk_for_mtdata;

   for(i = 1; i<=DSKINF.flgblkcnt; i++){
	
		memset(buffer, 0xFF, DSKINF.blksz);

      if(preoccupied_blocks > 0){
         occupblk_to_set = preoccupied_blocks < (DSKINF.blksz * VDBYTESZ) ? preoccupied_blocks : (DSKINF.blksz * VDBYTESZ);
         memset(buffer, 0x00, occupblk_to_set/VDBYTESZ);
         preoccupied_blocks -= occupblk_to_set;
         buffer[occupblk_to_set/VDBYTESZ] = ~((1<<(occupblk_to_set%VDBYTESZ))-1);
      }

		vdwrite(fd, buffer, i, DSKINF.blksz);

	}

   create_metadata_blocks(fd, DSKINF, buffer);   
   free(buffer);  
   close(fd);
   
   printf("Disk creation successfull\n");
   return 0;
}