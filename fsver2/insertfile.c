#include "vdconstants.h"
#include "vdrun_disk.h"
#include "vddiskinfo.h"
#include "vdsetbits.h"
#include "vdsyslib.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int get_emtmtdblk_loc(int fd, DISKINFO DSKINF, unsigned int *dskblk_ofmtd, unsigned int *loc_ofmtd_in_blk){
 
	int i, j;
	unsigned int ttl_mtdblks_in_dskblk = sizeof(FL_METADATA)/DSKINF.blksz;
	FL_METADATA *flmtdptr;
	
	char *buffer = malloc(sizeof(char) * DSKINF.blksz);
	lseek(fd, DSKINF.mtdta_blk_ofst*DSKINF.blksz, SEEK_SET);
	
	i = 0;
	while(i<DSKINF.ttlmtdta_blks){

		memset(buffer, 0, DSKINF.blksz);	
		vdread(fd, buffer, DSKINF.mtdta_blk_ofst + i, DSKINF.blksz);
		flmtdptr = (FL_METADATA *)buffer;
		
		for(j = 0; j<ttl_mtdblks_in_dskblk; j++){
			if(flmtdptr->isavailable){
				*dskblk_ofmtd = DSKINF.mtdta_blk_ofst + i;
				*loc_ofmtd_in_blk = j;
				return 0;
			}
			flmtdptr++;
		}

		i++;
	} 
	
	return -1;
}


int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int flbegblock, int disk_fd, DISKINFO DSKINF){
	
	// structure variable to store filematadat
	FL_METADATA flmtd;

	char *buffer = malloc(sizeof(char) *DSKINF.blksz);
	char *chptr = NULL;
	int i = 1;
	int j = 0;

	memset(buffer, '\0', DSKINF.blksz);
	vdread(disk_fd, buffer, blk, DSKINF.blksz);
	//store filename in structure variable 
	strcpy(flmtd.flnm, usrflnm);
	// set the metadata block as available
	// when the block is set as available 
	// it is considered as a valid block
	// if it is set to unavailable 
	// then it can be used to store data of some other file
	flmtd.strtloc = flbegloc;
	flmtd.flsz = usrflsz;

	//store bytes of structure variable in buffer at given location
	chptr = (char *)&flmtd;
	for(i = 0; i < sizeof(FL_METADATA); i++){
		buffer[loc_in_blk] = *chptr++;
	}

	// write metadata to disk
	vdwrite(disk_fd, buffer, blk, DSKINF.blksz);
	// set the bits as occupied for the block where metadata is written  
	setbits(blk, 1, 0);

	printf("filstatr loc %u\n", flmtd.strtloc);
	printf("filsz %u\n", flmtd.flsz);
	// increase the count of total metadatablocks that have been sotred
	DSKINF.ttlmetadata_blks += 1;

	memset(buffer, '\0', DSKINF.blksz);
	// read zeroth block
	vdread(disk_fd, buffer, 0, DSKINF.blksz);

	// write every byte of metadata to buffer at appopriate place
	chptr = (char *)&DSKINF.ttlmetadata_blks;
	printf("size is %d\n", buffer[0]);
	printf("size is %d\n", buffer[1]);
	buffer[2] = *chptr++;
	buffer[3] = *chptr++;
	buffer[4] = *chptr++;
	buffer[5] = *chptr;

	// write buffer to disk
	vdwrite(disk_fd, buffer, 0, DSKINF.blksz);
	printf("total metadata blocks in the end %d\n", DSKINF.ttlmetadata_blks);

	return 0;
}
*/


int insert( int usrfl_fd, int fd, DISKINFO DSKINF, 
			unsigned int *level, unsigned int depth, 
		    unsigned int block_int_capacity, FR_FLGBLK_LST *FFLST,  
		    unsigned int parent_block){

		int i, j, size;
		int val = level[depth];
		unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
		char *buffer = malloc(sizeof(char) * DSKINF.blksz);

		for(i = 0; i<level[depth]; i++){
			
			size = val > block_int_capacity ? block_int_capacity : val;
			getempty_blocks(size, blocks, FFLST);
			
			if(parent_block > 0){
				memset(buffer, 0, DSKINF.blksz);
				write_to_buffer(buffer, blocks, DSKINF.blksz, 0);
				vdwrite(fd, buffer, parent_block, DSKINF.blksz);
			}
			
			for(j = 0; j<size; j++){
				
				level[depth]--;			

				if(depth == 0){ 
					memset(buffer, 0, DSKINF.blksz);
					read(usrfl_fd, buffer, DSKINF.blksz);
					vdwrite(usrfl_fd, buffer, blocks[j], DSKINF.blksz);
				}else{
					depth--;	
					insert(usrfl_fd, fd, DSKINF, level, depth, blocks[j], block_int_capacity, FFLST);
				}
			}
			
			val -= size;
		}

		free(blocks);
		free(buffer);
}


int insert_file(int disk_fd, char *usrflnm, DISKINFO DSKINF, unsigned long int *flags, FR_FLGBLK_LST *FFLST){
	
	int usrfl_fd = opne(usrflnm, O_RDONLY, 00777);
	if (usrfl_fd == -1) return -1;

	unsigned long int usrflsz = lseek(usrfl_fd, 0, SEEK_END);
	int i = 0;
	int level_size[5];
	unsigned int filedata_blocks = ceil((float)usrflsz/(float)DSKINF.blksz);
	unsigned int block_int_capacity = (DSKINF.blksz/sizeof(int));
	int tree_depth = 0;
	int temp = filedata_blocks;
	unsigned int dskblk_ofmtd; 
	unsigned int loc_ofmtd_in_blk;
	
	lseek(usrfl_fd, 0, SEEK_SET);

	if(get_emtmtdblk_loc(disk_fd, DSKINF, &dskblk_ofmtd, &loc_ofmtd_in_blk) == -1)
		return -1;
	
	if(usrflsz > (FFLST->frblkcnt*DSKINF.blksz)){
		return -2;
	
	level_size[i++] = temp;

	do{
		temp = ceil((float)temp/(float)block_int_capacity);
		level_size[i++] = temp;
	}while(temp != 1);

	insert(usrfl_fd, disk_fd, DSKINF, level_size, tree_depth, 0, block_int_capacity, FFLST);

	//write_metadata(usrflnm, usrflsz, blocks[0], dsk_fd, blk, loc_in_blk);

	close(usrfl_fd);
	return 0;
}

