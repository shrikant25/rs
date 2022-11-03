
#include <stdlib.h>
#include <string.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vddriver.h"

void retrive_file_data(FILE_ACTION_VARS *FAV, unsigned int parent_block){

    int i, j, k;
    int size = 0;
    int *intptr = NULL;
    int val = FAV->level_size[FAV->tree_depth];
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
    char *buffer2 = malloc(sizeof(char) * FAV->DSKINF.blksz);

    for(i = 0; i<FAV->level_size[FAV->tree_depth]; i++){
        
        if(FAV->tree_depth > 0){
            size = val > block_int_capacity ? block_int_capacity : val;
            memset(blocks, 0, FAV->DSKINF.blksz);
            memset(buffer2, 0, FAV->DSKINF.blksz);
            vdread(FAV->disk_fd, buffer2, parent_block, FAV->DSKINF.blksz);
            
            intptr = (int *)buffer2;
            
            for(k = 0; k<size; k++){
                blocks[k] = *intptr++;
            }

            for(j = 0; j<size; j++){
            
                FAV->level_size[FAV->tree_depth]--;			
            
                FAV->tree_depth--;	
                retrive_file_data(FAV, blocks[j]);
            
            }
               val -= size;
        }
        else if(FAV->tree_depth == 0){ 
                size = 0;
                size = FAV->usrflsz > FAV->DSKINF.blksz ? FAV->DSKINF.blksz : FAV->usrflsz;  
                memset(buffer, 0, FAV->DSKINF.blksz);
                vdread(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);
                write(FAV->usrfl_fd, buffer, size);
               // printf("%s bufer :", buffer);
                FAV->usrflsz -= size;
                
        }
    }

    free(blocks);
    free(buffer);
    free(buffer2);
}


int fetch(FILE_ACTION_VARS *FAV){


    retrive_file_data(FAV, FAV->filebegloc);
    close(FAV->usrfl_fd);
}

