
#include <stdlib.h>
#include <string.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vddriver.h"

void retrive_file_data(FILE_ACTION_VARS *FAV, unsigned int parent_block, int tree_depth, unsigned int *blocks, 
                        char * buffer, char * buffer2){

    int i, j, k;
    int size = 0;
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    int *intptr = NULL;
    int val = FAV->level_size[tree_depth];

    for(i = 0; i<FAV->level_size[tree_depth]; i++){
        
        if(tree_depth > 0){
            size = val > block_int_capacity ? block_int_capacity : val;
            memset(blocks, 0, FAV->DSKINF.blksz);
            memset(buffer2, 0, FAV->DSKINF.blksz);
            vdread(FAV->disk_fd, buffer2, parent_block, FAV->DSKINF.blksz);
            
            intptr = (int *)buffer2;
            
            for(k = 0; k<size; k++){
                blocks[k] = *intptr++;
            }

            for(j = 0; j<size; j++){
            
                FAV->level_size[tree_depth]--;			
            
                tree_depth--;	
                retrive_file_data(FAV, blocks[j], tree_depth, blocks, buffer, buffer2);
            
            }
               val -= size;
        }
        else if(tree_depth == 0){ 
                size = 0;
                size = FAV->usrflsz > FAV->DSKINF.blksz ? FAV->DSKINF.blksz : FAV->usrflsz;  
                memset(buffer, 0, FAV->DSKINF.blksz);
                vdread(FAV->disk_fd, buffer, parent_block, FAV->DSKINF.blksz);
                write(FAV->usrfl_fd, buffer, size);
               // printf("%s bufer :", buffer);
                FAV->usrflsz -= size;
                
        }
    }

}


int fetch(FILE_ACTION_VARS *FAV){

    
    unsigned int block_int_capacity = FAV->DSKINF.blksz/sizeof(int);
    unsigned int *blocks = malloc(sizeof(int) * block_int_capacity);
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
    char *buffer2 = malloc(sizeof(char) * FAV->DSKINF.blksz);

    retrive_file_data(FAV, FAV->filebegloc, FAV->tree_depth, blocks, buffer, buffer2);
    free(blocks);
    free(buffer);
    free(buffer2);
    close(FAV->usrfl_fd);
}

