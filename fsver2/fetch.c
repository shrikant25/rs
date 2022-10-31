#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vdrun_disk.h"
#include "vddriver.h"
#include "vddiskinfo.h"
// still not done

void retrive_file_data(FILE_ACTION_VARS *FAV, unsigned int parent_block){

    int i, j;
    int size = 0;
    int val = FAV->level_size[FAV->tree_depth];
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);

    for(i = 0; i<FAV->level_size[FAV->tree_depth]; i++){
        
        size = val > block_int_capacity ? block_int_capacity : val;
        memset(blocks, 0, FAV->DSKINF.blksz);
        vdread(FAV->disk_fd, blocks, parent_block, FAV->DSKINF.blksz);
        
        for(j = 0; j<size; j++){
            
            FAV->level_size[FAV->tree_depth]--;			

            if(FAV->tree_depth == 0){ 
                memset(buffer, 0, FAV->DSKINF.blksz);
                vdread(FAV->disk_fd, buffer, blocks[j], FAV->DSKINF.blksz);
                vdwrite(FAV->usrfl_fd, buffer, FAV->DSKINF.blksz, FAV->DSKINF.blksz);
                
            }else{
                FAV->tree_depth--;	
                retrive_file_data(FAV, blocks[j]);
            }
        }
        
        val -= size;
    }

    free(blocks);
    free(buffer);
}


int fetch(FILE_ACTION_VARS *FAV){


    retrive_file_data(FAV, FAV->filebegloc);

    close(FAV->usrfl_fd);
}

