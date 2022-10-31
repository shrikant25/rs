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

int write_metadata( FILE_ACTION_VARS *FAV,  unsigned int flbegbloc){

	// structure variable to store filematadat
	FL_METADATA flmtd;

	char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
	char *chptr = NULL;

	memset(buffer, '\0', FAV->DSKINF.blksz);
	vdread(FAV->disk_fd, buffer, FAV->dskblk_ofmtd, FAV->DSKINF.blksz);
	//store filename in structure variable 
	strcpy(flmtd.flnm, FAV->usrflnm);
	flmtd.strtloc = flbegbloc;
	flmtd.flsz = FAV->usrflsz;
	flmtd.isavailable = 0;
		printf("filenmae %s\n", flmtd.flnm);
		printf(" %d\n", flmtd.flsz);
		printf("%d\n", FAV->dskblk_ofmtd);
		printf("%d\n", FAV->loc_ofmtd_in_blk);
	//store bytes of structure variable in buffer at given location
	
	write_to_buffer(buffer, (char *)&flmtd, sizeof(FL_METADATA), FAV->loc_ofmtd_in_blk);
	
	// write metadata to disk
	vdwrite(FAV->disk_fd, buffer, FAV->dskblk_ofmtd, FAV->DSKINF.blksz);
	
	free(buffer);
	return 0;
}

unsigned int insert( FILE_ACTION_VARS *FAV, unsigned int parent_block){

		int i, j, size;
		unsigned int root_block = 0;
		int val = FAV->level_size[FAV->tree_depth];
		unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
		unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
		char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

		for(i = 0; i<FAV->level_size[FAV->tree_depth]; i++){
			
			size = val > block_int_capacity ? block_int_capacity : val;
			memset(blocks, 0, FAV->DSKINF.blksz);
			getempty_blocks(size, blocks, FAV->FFLST);
			setbits(blocks, size, 0, FAV->flags);
			build(FAV->DSKINF, FAV->flags, FAV->FFLST);

			if(parent_block > 0){
				memset(buffer, 0, FAV->DSKINF.blksz);
				write_to_buffer(buffer, (char *)blocks, FAV->DSKINF.blksz, 0);
				vdwrite(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);
			}
			
			for(j = 0; j<size; j++){
				
				FAV->level_size[FAV->tree_depth]--;			

				if(FAV->tree_depth == 0){ 
					memset(buffer, 0, FAV->DSKINF.blksz);
					read(FAV->usrfl_fd, buffer, FAV->DSKINF.blksz);
					vdwrite(FAV->disk_fd, buffer, blocks[j], FAV->DSKINF.blksz);
				}else{
					FAV->tree_depth--;	
					insert(FAV, blocks[j]);
				}
			}
			
			val -= size;
		}

		root_block = blocks[0];
		free(blocks);
		free(buffer);
		return root_block;
}


int insert_file( FILE_ACTION_VARS *FAV){
	
	unsigned int filebegblk = insert(FAV, 0);
	write_metadata(filebegblk, FAV);
	
	printf("yeah");
	return 0;
}

