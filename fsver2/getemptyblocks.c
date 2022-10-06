#include<stdio.h>
#include "vdheader.h"
#include <stdlib.h>
#include <math.h>


void insertblk(FR_FLGBLK *newfr_blk){

	FR_FLGBLK **temp = &(FFLST.head);
	FFLST.frblkcnt++;
	printf(" FFLST.frblkcnt %d\n", FFLST.frblkcnt);
	while(*temp != NULL){
		
		if((*temp)->cnt > newfr_blk->cnt)
			temp = &(*temp)->next;
	
		else break;
	}
	newfr_blk->next = *temp;
	*temp = newfr_blk;

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

