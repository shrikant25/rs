#include <stdlib.h>
#include <string.h>
#include "vdsetbits.h"
#include <stdio.h>
#include "vdsyslib.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"

void insert(FILE_ACTION_VARS *FAV, unsigned int parent_block, int cur_tree_depth){

    int j, k;
    int size = 0;

	int data_read;
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int); 
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

    size = FAV->level_size[cur_tree_depth] < block_int_capacity ? FAV->level_size[cur_tree_depth] : block_int_capacity;
	
	memset(blocks, 0, FAV->DSKINF.blksz);
	getempty_blocks(size, blocks, FAV->FFLST);
	
	setbits(blocks, size, 0, FAV->flags);
	build(FAV->DSKINF, FAV->flags, FAV->FFLST);

	memset(buffer, 0, FAV->DSKINF.blksz);
	write_to_buffer(buffer, (char *)blocks, FAV->DSKINF.blksz, 0);
	vdwrite(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);

    for(j = 0; j<size; j++){
    
        if(cur_tree_depth > 0){			
            insert(FAV, blocks[j], (cur_tree_depth-1));
        }
        else{ 
            memset(buffer, 0, FAV->DSKINF.blksz);
			data_read = read(FAV->usrfl_fd, buffer, FAV->DSKINF.blksz);
			vdwrite(FAV->disk_fd, buffer, blocks[j], FAV->DSKINF.blksz);
        }
    }

    FAV->level_size[cur_tree_depth] -= size;

    free(blocks);
    free(buffer);
}


int insert_file( FILE_ACTION_VARS *FAV){
	
	FL_METADATA flmtd;
	
	unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int); 
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    
	memset(blocks, 0, FAV->DSKINF.blksz);
	getempty_blocks(1, blocks, FAV->FFLST);
	flmtd.strtloc =  blocks[0];
	
	setbits(blocks, 1, 0, FAV->flags);
	build(FAV->DSKINF, FAV->flags, FAV->FFLST);
	free(blocks);
	
	insert(FAV, flmtd.strtloc, FAV->tree_depth);
	
	strncpy(flmtd.flnm, FAV->usrflnm, strlen(FAV->usrflnm));
	flmtd.flnm[strlen(FAV->usrflnm)] ='\0';
	
	flmtd.flsz = FAV->usrflsz;
	flmtd.isavailable = 0;
	write_metadata(FAV, flmtd);
	
	return 0;
}

