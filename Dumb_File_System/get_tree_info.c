#include <math.h>
#include "vdfile_metadata.h"

void get_tree_info(FILE_ACTION_VARS *FAV){

	unsigned int block_int_capacity = (FAV->DSKINF.blksz/sizeof(int));
	unsigned int temp = ceil((float)FAV->usrflsz/(float)FAV->DSKINF.blksz);
    FAV->tree_depth = 0;
	
	FAV->level_size[FAV->tree_depth] = temp;

	do{
		temp = ceil((float)temp/(float)block_int_capacity);
		FAV->level_size[++FAV->tree_depth] = temp;
	}while(temp != 1);

}