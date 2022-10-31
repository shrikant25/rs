#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vdrun_disk.h"
#include "vddriver.h"
#include "vddiskinfo.h"

void set_blocks_free(FILE_ACTION_VARS *FAV, unsigned int parent_block){

    int i, j;
    int size;
    int val = FAV->level_size[FAV->tree_depth];
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

    for(i = 0; i<FAV->level_size[FAV->tree_depth]; i++){
        
        size = val > block_int_capacity ? block_int_capacity : val;
        memset(blocks, 0, FAV->DSKINF.blksz);
        vdread(FAV->disk_fd, blocks, parent_block, FAV->DSKINF.blksz);
        setbits(blocks, size, 0, FAV->flags);
        
        if(FAV->tree_depth > 0){
            for(j = 0; j<size; j++){
                
                FAV->level_size[FAV->tree_depth]--;			

                    FAV->tree_depth--;	
                    set_blocks_free(FAV, blocks[j]);
            }
        }
        
        val -= size;
    }

    free(blocks);
    free(buffer);
}


int delete(FILE_ACTION_VARS *FAV){

    FL_METADATA flmtd;
	strcpy(flmtd.flnm, "");
	flmtd.strtloc = 0;
	flmtd.flsz = 0;
	flmtd.isavailable = 1;

	write_metadata(FAV, filebegblk, flmtd);
    set_blocks_free(FAV, parent_block);
    free(before);
}