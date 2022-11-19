
#include <stdlib.h>
#include <string.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vddriver.h"

void retrive_file_data(FILE_ACTION_VARS *FAV, unsigned int parent_block, int cur_tree_depth){

    int j, k;
    int size = 0;
    int data_to_be_read = 0;
    int data_read;
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

    for(j = 0; j<size; j++){
    
        if(cur_tree_depth > 0){			
            retrive_file_data(FAV, blocks[j], (cur_tree_depth-1));
        }
        else{ 

            data_to_be_read = 0;
            data_to_be_read = FAV->usrflsz > FAV->DSKINF.blksz ? FAV->DSKINF.blksz : FAV->usrflsz;  
            memset(buffer, 0, FAV->DSKINF.blksz);
            vdread(FAV->disk_fd, buffer, blocks[j], FAV->DSKINF.blksz);
            data_read = write(FAV->usrfl_fd, buffer, data_to_be_read);
            FAV->usrflsz -= data_to_be_read;
        }
    }

    FAV->level_size[cur_tree_depth] -= size;
    free(blocks);
    free(buffer);
}


int fetch(FILE_ACTION_VARS *FAV){

    retrive_file_data(FAV, FAV->filebegloc, FAV->tree_depth);
    
    close(FAV->usrfl_fd);
}

