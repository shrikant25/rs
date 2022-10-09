#include<stdio.h>
#include "vdheader.h"
#include <stdlib.h>
#include <math.h>


void insertblk(FR_FLGBLK *newfr_blk){

	FR_FLGBLK **temp = &(FFLST.head);

	while(*temp != NULL){
		
		if((*temp)->cnt > newfr_blk->cnt)
			temp = &(*temp)->next;
	
		else break;
	}
	newfr_blk->next = *temp;
	*temp = newfr_blk;

	FFLST.frblkcnt += newfr_blk->cnt;
	printf(" FFLST.frblkcnt %d\n", FFLST.frblkcnt);
}
	
FR_FLGBLK * createblk(unsigned int bitloc){

	FR_FLGBLK * blk = (FR_FLGBLK *)malloc(sizeof(FR_FLGBLK));
	blk->loc = (unsigned int)bitloc;
	blk->cnt = 0;

	return blk;
}

void display_lst(){
	
	FR_FLGBLK **temp = &(FFLST.head);

	while(*temp != NULL){
		printf("unsigned int loc %u\n", (*temp)->loc);
		printf("unsigned int cnt %u\n", (*temp)->cnt);
	 printf("\n\n");
		temp = &(*temp)->next;
	}
}


void build(int chunks){

	int i,j;
	unsigned long int temp = 0;
	FR_FLGBLK *fr_flgblk = NULL;	
	
	for(i=7; i<chunks; i++){

		if(flags[i]){
			for(j=0; j<ULBCNT; j++){
					
				if((flags[i]>>j) & 1){
					
					if(!fr_flgblk){
						printf("i %d, j %d, val %d",i,j, (i*ULBCNT)+j);
						fr_flgblk = createblk((i*ULBCNT)+j);
						
					}
					fr_flgblk->cnt++;
				}
				else{ 
					if(fr_flgblk)
						insertblk(fr_flgblk);
					fr_flgblk = NULL;
				}
			}	
		}
		else{
			 
			if(fr_flgblk)
				insertblk(fr_flgblk);
			 fr_flgblk = NULL;
		}
	}	
	if(fr_flgblk)
		insertblk(fr_flgblk);	
	display_lst();
}


int getempty_blocks(int blocks_required, unsigned int *blocks){

	if(blocks_required > FFLST.frblkcnt || blocks_required <= 0) return -1;

	FR_FLGBLK **temp = NULL;
	unsigned int available_blocks_count = 0;
	int i, j;
	unsigned int block_in_series;

	i = 0;
	j = 0;

	do{
		if(j == 0){
			temp = (temp == NULL) ? &FFLST.head : &(*temp)->next;
			j = (*temp)->cnt;
			block_in_series = (*temp)->loc;
			printf("bli in serisk %d\n", block_in_series);
		}
		
		blocks[i++] = block_in_series++;
		j--;
		blocks_required--;
	
	}while(blocks_required > 0);

	return 0;
}
