#include <stdio.h>
#include <math.h>
#include "vdconstants.h"

int setbits(unsigned int *beg_loc, int total_blocks_required, int bitsign, unsigned long int *flags){ 

	int i;
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int block_num = 0;
	
	for(i = 0; i<total_blocks_required; i++){
		
		block_num = beg_loc[i]+1;
		
		flag = ceil((float)block_num/(float)VDQUAD);
		startbit_inflag = block_num - ((flag-1) * VDQUAD);
		flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
		   
	}	
}
