#include <math.h>
#include "vdheader.h"

int setbits(unsigned int beg_loc, unsigned long int bytes, unsigned int bitsign){ 
	
	
	unsigned int bitcnt = ceil((float)bytes/(float)DSKINF.blksz);
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int loc = beg_loc;
	unsigned int num_bits_manp = 0;
	
	while(bitcnt){
	
		flag = ceil((float)loc/(float)64);
		startbit_inflag = loc - ((flag-1) * ULBCNT);
		loc -= startbit_inflag;
	
		if(startbit_inflag == 1 && bitcnt>=64){
			flags[flag-1] = 0xFFFFFFFFFFFFFFFF * bitsign;
			startbit_inflag = 65;
			bitcnt -= 64;
		}
		else{
	        	while(startbit_inflag<=64 && bitcnt>0){
				flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
				bitcnt--;            
				startbit_inflag++;		
			}                                      
		}		
		       				       
		loc += startbit_inflag;
	}

}
