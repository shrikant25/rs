
#include <stdlib.h>
#include <string.h>
#include "vdsyslib.h"
#include "vdsetbits.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"
#include <stdio.h>

void set_blocks_free(FILE_ACTION_VARS *FAV, unsigned int parent_block, int cur_tree_depth){

    int j, k;
    int size = 0;
    int data_to_be_read = 0;
    int *intptr = NULL;

    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int); 
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
    
    memset(blocks, 0, FAV->DSKINF.blksz);
    memset(buffer, 0, FAV->DSKINF.blksz);
    vdread(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);

    intptr = (int *)buffer;
    size = FAV->level_size[cur_tree_depth] < block_int_capacity ? FAV->level_size[cur_tree_depth] : block_int_capacity;
    
    for(k = 0; k<size; k++){
        blocks[k] = *intptr++;
    }

    setbits(blocks, size, 1, FAV->flags);
    
    if(cur_tree_depth > 0){
        for(j = 0; j<size; j++){            
            set_blocks_free(FAV, blocks[j], (cur_tree_depth-1));
        }
    }

    FAV->level_size[cur_tree_depth] -= size;
    free(blocks);
    free(buffer);
}


int delete(FILE_ACTION_VARS *FAV){

    FL_METADATA flmtd;
	strcpy(flmtd.flnm, "");
	flmtd.strtloc = -1;
	flmtd.flsz = -1;
	flmtd.isavailable = 1;

	write_metadata(FAV, flmtd);
    setbits(&FAV->filebegloc, 1, 1, FAV->flags);
    set_blocks_free(FAV, FAV->filebegloc, FAV->tree_depth);
    
}