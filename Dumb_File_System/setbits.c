#include <stdio.h>
#include <math.h>
#include "vdconstants.h"

int setbits(unsigned int *blocks_list, int total_blocks_count, int bitsign, unsigned long int *flags){ 

	int i;
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int block_num = 0;
	
	for(i = 0; i<total_blocks_count; i++){
	
		block_num = blocks_list[i]+1;
		//printf("blk num%d  bit %d \n", block_num, bitsign);
		flag = ceil((float)block_num/(float)(VDQUAD * VDBYTESZ));
		startbit_inflag = block_num - ((flag-1) * (VDQUAD* VDBYTESZ));
		flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
	}	
}
