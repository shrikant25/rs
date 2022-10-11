#include <stdio.h>
#include <math.h>
#include "vdheader.h"

int setbits(unsigned int *beg_loc, int total_blocks_required, int bitsign){ 

	int i;
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int block_num = 0;
	
	for(i = 0; i<total_blocks_required; i++){
		
		block_num = beg_loc[i]+1;
		
		flag = ceil((float)block_num/(float)ULBCNT);
		startbit_inflag = block_num - ((flag-1) * ULBCNT);
	//	printf("ohh %u %d\n", flag, block_num);
		flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
	//	printf("ohh222222\n");
		//printf("\nafter %lx\n", flags[flag-1]);
		   
	}	
	
}
