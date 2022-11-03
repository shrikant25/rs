
#include <stdlib.h>
#include <string.h>
#include "vdsyslib.h"
#include "vdsetbits.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"


void set_blocks_free(FILE_ACTION_VARS *FAV, unsigned int parent_block, int tree_depth, unsigned int * blocks, char * buffer){

    int i, j, k;
    int size;
    int *intptr = NULL;
    int val = FAV->level_size[tree_depth];
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);

    for(i = 0; i<FAV->level_size[tree_depth]; i++){
        
        size = val > block_int_capacity ? block_int_capacity : val;
        memset(blocks, 0, FAV->DSKINF.blksz);
        memset(buffer, 0, FAV->DSKINF.blksz);
        vdread(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);
        
        intptr = (int *)buffer;
        
        for(k = 0; k<size; k++){
            blocks[k] = *intptr++;
        }
        
        setbits(blocks, size, 0, FAV->flags);
        
        if(tree_depth > 0){
            for(j = 0; j<size; j++){
                
                FAV->level_size[tree_depth]--;			

                    FAV->tree_depth--;	
                    set_blocks_free(FAV, blocks[j], tree_depth, blocks, buffer);
            }
        }
        
        val -= size;
    }

}


int delete(FILE_ACTION_VARS *FAV){

    FL_METADATA flmtd;
	strcpy(flmtd.flnm, "");
	flmtd.strtloc = 0;
	flmtd.flsz = 0;
	flmtd.isavailable = 1;
    
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

	write_metadata(FAV, flmtd);
    set_blocks_free(FAV, FAV->filebegloc, FAV->tree_depth, blocks, buffer);

    free(blocks);
    free(buffer);
}