#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define bitinlong 64
#define blksz 1024

unsigned long flags[10]= { 0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
		           0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
			   0xFFFFFFFFFFFFFFFF,
		           0xFFFFFFFFFFFFFFFF
			};



// some higher func will send the location of block(inclusive, indexing start at 1) from which the data was written
// aswell as the count of bytes that were written

int setbits(int loc, unsigned long int bytes, int bitsign){ 

	int bitcnt = ceil((float)bytes/(float)blksz);
	int flag = 0;
	int startbit_inflag = 0;
	int num_bits_manp = 0;

	while(bitcnt){

		flag = ceil((float)loc/(float)bitinlong);
		startbit_inflag = loc - ((flag-1) * bitinlong);
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


int main(){
	
	for(int j =0; j<10; j++){
		printf("%d = %lx\n", j, flags[j]);
	}

	printf("\n\n");
	setbits(127, (200 * 1024), 0);

	for(int j =0; j<10; j++){
		printf("%d = %lx\n", j, flags[j]);
	}


	printf("\n\n");
	setbits(127, (200 * 1024), 1);
	
	for(int j =0; j<10; j++){
		printf("%d = %lx\n", j, flags[j]);
	}
}
