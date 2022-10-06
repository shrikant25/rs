#include <stdio.h>
#include <math.h>
#include "vdheader.h"

int setbits(unsigned int beg_loc, unsigned long int bytes, unsigned int bitsign){ 
	
	
	unsigned int bitcnt = ceil((float)bytes/(float)DSKINF.blksz);
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int loc = beg_loc;
	loc += 1; // dont want data to start at 0
	unsigned int num_bits_manp = 0;
	
	printf("\nbitcnt %d\n", bitcnt);
    
	while(bitcnt){
	
		flag = ceil((float)loc/(float)ULBCNT);
	printf("\nbefore %lx\n", flags[flag-1]);
	
		startbit_inflag = loc - ((flag-1) * ULBCNT);
		loc -= startbit_inflag;

		if(startbit_inflag == 1 && bitcnt>=ULBCNT){
			flags[flag-1] = 0xFFFFFFFFFFFFFFFF * bitsign;
			startbit_inflag = 65;
			bitcnt -= 64;
		}
		else{
	        	while(startbit_inflag<=ULBCNT && bitcnt>0){
				flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
				bitcnt--;            
				startbit_inflag++;		
			}                                      
		}		
		       				       
		loc += startbit_inflag;
	}
	printf("\nafter %lx\n", flags[flag-1]);

}
