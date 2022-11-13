#include <stdio.h>
#include "vdrun_disk.h"
#include "vdconstants.h" 
#include <stdlib.h>
#include "vddiskinfo.h"
#include <math.h>

void insertblk(FR_FLGBLK *newfr_blk, FR_FLGBLK_LST *FFLST){

	FR_FLGBLK **temp = &(FFLST->head);

	while(*temp != NULL){
		
		if((*temp)->cnt > newfr_blk->cnt)
			temp = &(*temp)->next;
	
		else break;
	}
	newfr_blk->next = *temp;
	*temp = newfr_blk;

	FFLST->frblkcnt += newfr_blk->cnt;
}
	

FR_FLGBLK * createblk(unsigned int bitloc){

	FR_FLGBLK * blk = (FR_FLGBLK *)malloc(sizeof(FR_FLGBLK));
	blk->loc = (unsigned int)bitloc;
	blk->cnt = 0;

	return blk;
}

void deallocate(FR_FLGBLK_LST *FFLST){

	FR_FLGBLK **temp = &FFLST->head;
	FR_FLGBLK *current = NULL;
	
	while(*temp != NULL){
		current = *temp;
		temp = &(*temp)->next;
		free(current);
	}

	FFLST->head = NULL;
}


void build(DISKINFO DSKINF, unsigned long int * flags, FR_FLGBLK_LST *FFLST){

	int i,j;
	unsigned long int temp = 0;
	deallocate(FFLST);
	FR_FLGBLK *fr_flgblk = NULL;	
	unsigned int flags_chunks = (DSKINF.flgblkcnt * DSKINF.blksz)/(VDQUAD); 
	
	for(i=0; i<flags_chunks; i++){
		if(flags[i]){
			for(j=0; j<(VDQUAD * VDBYTESZ); j++){
					
				if((flags[i]>>j) & 1){
					if(!fr_flgblk)
						fr_flgblk = createblk((i*(VDBYTESZ * VDQUAD))+j);
					fr_flgblk->cnt++;
				}
				else{ 
					if(fr_flgblk)
						insertblk(fr_flgblk, FFLST);
					fr_flgblk = NULL;
				}
			}	
		}
		else{
			 
			if(fr_flgblk)
				insertblk(fr_flgblk, FFLST);
			 fr_flgblk = NULL;
		}
	}	
	if(fr_flgblk)
		insertblk(fr_flgblk, FFLST);	
	
}


int getempty_blocks(int blocks_required, unsigned int *blocks, FR_FLGBLK_LST *FFLST){

	if(blocks_required > FFLST->frblkcnt || blocks_required <= 0) return -1;

	FR_FLGBLK **temp = NULL;
	unsigned int available_blocks_count = 0;
	int i, j;
	unsigned int block_in_series;

	i = 0;
	j = 0;

	do{
		if(j == 0){
			temp = (temp == NULL) ? &FFLST->head : &(*temp)->next;
			j = (*temp)->cnt;
			block_in_series = (*temp)->loc;
		}
		
		blocks[i++] = block_in_series++;
		j--;
		blocks_required--;
	
	}while(blocks_required > 0);

	return 0;
}
