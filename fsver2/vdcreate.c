#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "vdheader.h"
#include "vdsyslib.h"


int create_metadata_blocks(fd, total_metadata_blocks, metadata_block_offset, blksz){
   
   unsigned int i,j;
   char *buffer = malloc(sizeof(char) * blksz);
   unsigned int ttl_mtdblks_in_dskblk = sizeof(FL_METADATA)/blksz;
   FL_METADATA flmtd;

   strcpy(flmtd.flnm, NULL);
   flmtd.strtloc = 0;
	flmtd.flsz = 0;
	flmtd.isavailable = 1;

   memset(buffer, '\0', blskz);

   for(j = 0; j<ttl_mtdblks_in_dskblk; j++){
      write_to_buffer(buffer, flmtd, sizeof(FL_METADATA), sizeof(FL_METADATA)*j);
   }

   for(i = metadata_block_offset; i<total_metadata_blocks; i++){
      vdwrite(fd, buffer, i, blksz);
   }

   return 0;

}

int set_blkflgs(int fd, unsigned int blkcnt,unsigned int preoccupied_blocks, unsigned int blksz){
  
   int i;
   unsigned char *buffer = malloc(sizeof(char) * blksz);
   int status = 0;
   int blocks_to_write = ceil((double)blkcnt/(8*blksz));
   int bytes_to_write = 0;
  
   for(i = 1; i<=blocks_to_write; i++){
		
      memset(buffer, 0x00, bytes_to_write);
      bytes_to_write = (blkcnt/8) > blksz ?  blksz : blkcnt/8; 
      memset(buffer, 0xFF, bytes_to_write);
      
      status = vdwrite(fd, buffer, i, blksz);
   	if(status <= 0)
   		return -1;

      blkcnt -= (bytes_to_write*8);
   }

   return 0;
   
}


int main(int argc, char *argv[]){
   
   
   if(argc != 4){
      write(1, "Invalid arguments\n", 18);
      exit(EXIT_FAILURE);
   }
   
   int i;
   int fd = 0;
   unsigned int val = 0;
   short int flgsts = 0;
   unsigned long int dsksz = 0;
   unsigned int dsksz_exp = 0;
   unsigned long int blksz = 0;
   unsigned int blksz_exp = 0;
   unsigned long int total_metadata_blocks
   unsigned long int blkcnt = 0;
   unsigned int flgblkcnt = 0;
   unsigned int metadata_block_offset = 0;
   unsigned char *buffer;
   unsigned int preoccupied_blocks = 0;
   unsigned int *preoccupied_blocks_list;

   dsksz_exp = atoi(argv[2]);
   blksz_exp = atoi(argv[3]);
   dsksz = pow(2, dsksz_exp);
   blksz = pow(2, blksz_exp);
   buffer = calloc((blksz), sizeof(char));

   blkcnt = dsksz/blksz;
   flgblkcnt = (blkcnt/blksz)/8;
   total_metadata_blocks = dsksz/(64*1024);
   dsk_blk_for_mtdata = (sizeof(FL_METADATA)*total_metadata_blocks)/blksz;
   metadata_block_offset = flgblkcnt + 1; // after flagblocks 

   printf("flag blocks count : %d\n", flgblkcnt);
   printf("total metadata blocks  %ld\n", total_metadata_blocks);
   printf("dsk blocks required for metadata blocks %ld\n", dsk_blk_for_mtdata);

   createfile(argv[1], dsksz);

   fd = open(argv[1], O_WRONLY, 00777);
  
   if(fd == -1){
      write(1, "Disk creation failed\n", 21);
      exit(EXIT_FAILURE);
   }
   
   write_to_buffer(buffer, (char *)&dsksz, 8, 0);
   write_to_buffer(buffer, (char *)&blksz, 8, 8);
   write_to_buffer(buffer, (char *)&total_metadata_blocks, 8, 15);
   write_to_buffer(buffer, (char *)&metadata_block_offset, 4, 23);

   vdwrite(fd, buffer, 0, blksz);
  
   flgsts = set_blkflgs(fd, blkcnt, blksz); 
	   
   //first block will be used to store other metadata
   preoccupied_blocks = 1 + flgblkcnt + total_metadata_blocks;
   if(flgsts){
   	write(1, "flag setting failed\n", 20);
	   exit(EXIT_FAILURE);
   }

   for(i = 0; i<preoccupied_blocks; i++){
      preoccupied_blocks_list[i] = i;
   }

   // mark preoccupied blocks as full
   setbits(preoccupied_blocks_list, preoccupied_blocks, 0x0);

   // write metadata blocks
   create_metadata_blocks(fd, total_metadata_blocks, metadata_block_offset, blksz);
   
   write(1, "Disk creation successfull\n", 26);
 
   close(fd);
   
   return 0;
}
