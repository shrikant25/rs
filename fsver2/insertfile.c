#include <stdlib.h>
#include <string.h>
#include "vdsetbits.h"
#include <stdio.h>
#include "vdsyslib.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"



unsigned int insert( FILE_ACTION_VARS *FAV, unsigned int parent_block, int tree_depth, unsigned int *blocks, char * buffer){

		int i, j, size;
		unsigned int root_block = 0;
		if(parent_block > 0)
			tree_depth--;	
		int val = FAV->level_size[tree_depth];
		unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
		
		for(i = 0; i<FAV->level_size[tree_depth]; i++){
			
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
				
				FAV->level_size[tree_depth]--;			

				if(tree_depth == 0){ 
					memset(buffer, 0, FAV->DSKINF.blksz);
					read(FAV->usrfl_fd, buffer, FAV->DSKINF.blksz);
					vdwrite(FAV->disk_fd, buffer, blocks[j], FAV->DSKINF.blksz);
				}else{
					insert(FAV, blocks[j], tree_depth, blocks, buffer);
				}
			}
			
			val -= size;
		}

		root_block = blocks[0];
		return root_block;
}


int insert_file( FILE_ACTION_VARS *FAV){
	
	unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
	unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
	char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

	unsigned int filebegblk = insert(FAV, 0, FAV->tree_depth, blocks, buffer);
	free(blocks);
	free(buffer);

	FL_METADATA flmtd;
	
	strncpy(flmtd.flnm, FAV->usrflnm, strlen(FAV->usrflnm));
	flmtd.flnm[strlen(FAV->usrflnm)] ='\0';
	printf("%s %ld",flmtd.flnm, strlen(flmtd.flnm));
	flmtd.strtloc = filebegblk;
	flmtd.flsz = FAV->usrflsz;
	flmtd.isavailable = 0;
	printf("done");
	write_metadata(FAV, flmtd);
	
	return 0;
}

