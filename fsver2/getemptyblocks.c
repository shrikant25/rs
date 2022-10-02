#include "vdheader.h"
#include <stdlib.h>
#include <math.h>


void insertblk(FR_FLGBLK *newfr_blk){

	if(FFLST.head == NULL){
		FFLST.frblkcnt = 1;
		FFLST.head = newfr_blk;
	}
	else{
	
		FFLST.frblkcnt++;
		
		if(FFLST.head->cnt < newfr_blk->cnt){
			newfr_blk->next = FFLST.head;
			FFLST.head = newfr_blk;	
		}
		else{
			newfr_blk->next = FFLST.head->next;
			FFLST.head->next = newfr_blk;
		}
	}
}


FR_FLGBLK * createblk(unsigned int bitloc){

	FR_FLGBLK * blk = (FR_FLGBLK *)malloc(sizeof(FR_FLGBLK));
	blk->loc = (unsigned int)bitloc;
	blk->cnt = 0;

	return blk;
}


void build(int chunks){

	int i,j;
	unsigned long int temp = 0;
	FR_FLGBLK *fr_flgblk = NULL;
	
	for(i=0; i<chunks; i++){

		if(flags[i]){
			for(j=0; j<ULBCNT; j++){
					
				if((flags[i]>>j) & 1){
					
					if(!fr_flgblk){
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

}

