#include <stdlib.h>
#include <string.h>
#include "vdsetbits.h"
#include "vdsyslib.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"



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

	FL_METADATA flmtd;
	strcpy(flmtd.flnm, FAV->usrflnm);
	flmtd.strtloc = filebegblk;
	flmtd.flsz = FAV->usrflsz;
	flmtd.isavailable = 0;

	write_metadata(FAV, flmtd);
	
	return 0;
}

