#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<math.h>
#define BUFLEN 1024
#define ULBCNT sizeof(unsigned long int)*8 // number of bits in unsigned long


typedef struct FR_FLGBLK{
	unsigned int loc;
	unsigned int cnt;
}FR_FLGBLK;


typedef struct{
	FR_FLGBLK *arr;
	unsigned int frblkcnt;
	unsigned int max_frblkcnt;
}FR_FLGBLK_LST;


FR_FLGBLK_LST FFLST;
unsigned long int dsksz;
unsigned long int blksz;
unsigned long int *flags;
unsigned int blkcnt;
unsigned int flgblkcnt;
unsigned int chunks;
const char * diskname;
unsigned int ttlmetadata_blks;


void display();
void display_flags();
void insert();
FR_FLGBLK * createblk(unsigned int);
void insertblk(unsigned int, unsigned int);
void build();
void swap(int a, int b);


void display(){

	int i;
	
	printf("total blcks %u\n", FFLST.frblkcnt);
	for(i = 0; i<FFLST.frblkcnt; i++){
		printf("location :%d \n", FFLST.arr[i].loc);
		printf("cnt : %d\n", FFLST.arr[i].cnt);
		printf("\n\n");
	}
}


void swap(int a, int b){

	FR_FLGBLK temp = FFLST.arr[a];
	FFLST.arr[a] = FFLST.arr[b];
	FFLST.arr[b] = temp;
}


void heapify(int large){

	if(FFLST.frblkcnt == 1)
		return;
		
	int largest = large;
	int left = 2 * large + 1;
	int right = 2 * large + 2;
	
	if(left < FFLST.frblkcnt && FFLST.arr[left].cnt > FFLST.arr[largest].cnt)
		largest = left;
	
	if(right < FFLST.frblkcnt && FFLST.arr[right].cnt > FFLST.arr[largest].cnt)
		largest = right;
		
	if(largest != large){
		printf("before swap %d\n", FFLST.arr[large].cnt);
		printf("before swap %d\n", FFLST.arr[largest].cnt);
		swap(large, largest);
		
	printf("after swap %d\n", FFLST.arr[large].cnt);
		printf("before swap %d\n", FFLST.arr[largest].cnt);
		heapify(largest);
	} 

}



void deleteblk(){

	int i = 0;
	swap(1, FFLST.frblkcnt-1);
	
	for(i = FFLST.frblkcnt / 2 -1; i>=0; i++)
		heapify(i); 
}


void insertblk(unsigned int bitcnt, unsigned int loc){

	int i = 0;
	
	if(FFLST.frblkcnt == 0){
		FFLST.frblkcnt += 1;
		FFLST.arr[0].cnt = bitcnt;
		FFLST.arr[0].loc = loc;
	}
	else{
	
		FFLST.arr[FFLST.frblkcnt].cnt = bitcnt;
		FFLST.arr[FFLST.frblkcnt].loc = loc;
		FFLST.frblkcnt += 1;
		
		for(i = FFLST.frblkcnt/2 - 1; i>=0; i--)
			heapify(i); 
	}
}



void build(){

	int i,j;
	unsigned long int temp = 0;
	FR_FLGBLK *fr_flgblk = NULL;
	unsigned int bitcnt = 0;
	unsigned int loc = 0;
	
	for(i=0; i<chunks; i++){

		if(flags[i]){
			for(j=0; j<ULBCNT; j++){
					
				if((flags[i]>>j) & 1){
					
					if(!bitcnt)
						loc = (i*ULBCNT)+j;
					
					bitcnt++;
				}
				else{ 
					if(bitcnt)
						insertblk(bitcnt, loc);
					bitcnt = 0;
					loc = 0;
				}
			}	
		}
		else{
			 
			if(bitcnt)
				insertblk(bitcnt, loc);
			bitcnt = 0;
			loc = 0;
		}
	}	
	
	if(bitcnt)
		insertblk(bitcnt, loc);
	
}


int main(unsigned int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}
	
	diskname = argv[1];
	char ch;
	unsigned int fd = open(diskname, O_RDONLY);
	unsigned int rdcnt;
	unsigned int readsize;
	unsigned long int usrflsz;

	if(fd){
		read(fd, &ch, sizeof(char));
		dsksz = (unsigned long int)pow(2, ch);

		read(fd, &ch, sizeof(char));
		blksz = (unsigned long int)pow(2, ch);
	
		blkcnt = dsksz/blksz;
		flgblkcnt = (blkcnt/8) / blksz; 
		
		FFLST.max_frblkcnt = blkcnt/2;
		FFLST.arr = (FR_FLGBLK *)malloc(sizeof(FFLST.max_frblkcnt) * (blkcnt/2)) ;
		FFLST.frblkcnt = 0;
	
		readsize = flgblkcnt * blksz;	
		readsize -= (flgblkcnt/(ULBCNT/8));
		chunks = readsize/(ULBCNT/8);
	
		flags = malloc(readsize);
		rdcnt = read(fd, flags, readsize);
		
		lseek(fd, -5, SEEK_END);
		read(fd, &ttlmetadata_blks, sizeof(ttlmetadata_blks));
		printf("total metadata blocks in the begining %u\n", ttlmetadata_blks);
		close(fd);



		if(rdcnt == readsize)
			build();
		else{
			perror("Failed to read flags\n");
			exit(EXIT_FAILURE);
		}
		
		display();
		printf("\n\n");		
				
		flags[22]    =  0xFFFFFFF0FFFFFFF0;
		flags[21]    =  0x0FFFFFF000000000;
		flags[30000] =  0x0FFFFFF0FFFF0FF0;
		flags[64000] =  0xFFFFFFF0FFFFFFFF;
		flags[3121]  =  0xFFFFFFF0FFFFFFFF;
		flags[512]   =  0xFFF00FF0FFFFFFFF;
		flags[1000]  =  0xFF000FFF0FFFFFFF;
		flags[1]     =  0x0000000000000000;
		flags[3111]  =  0xFFF00FF0FFFFFF0F;
	FFLST.arr = (FR_FLGBLK *)malloc(sizeof(FFLST.max_frblkcnt) * (blkcnt/2)) ;
		FFLST.frblkcnt = 0;
		build();
		display();


	//for(int i=0;i<readsize/(ULBCNT/8);i++)
	//		flags[i] = 0x5555555555555555;

/*

		flags[26000]    =  0xFFFFFFF0FFFFFFF0;
		flags[27000]    =  0x0FFFFFF000000000;
		flags[30000]    =  0x0FFFFFF0FFFF0FF0;
		flags[64000]    =  0xFFFFFFF0FFFFFFFF;
		flags[31210]    =  0xFFFFFFF0FFFFFFFF;
		flags[51200]    =  0xFFF00FF0FFFFFFFF;
		flags[25000]    =  0xFF000FFF0FFFFFFF;
		flags[25110]    =  0x0000000000000000;
		flags[31110]    =  0xFFF00FF0FFFFFF0F;

*/
		
		
		
		
		/*
		printf("total blcks %d\n", FFLST.frblkcnt);
		
		printf("largest available block is at %d\n", FFLST.head->loc+flgblkcnt+1);
		printf("largest available block is at %ld\n", ((FFLST.head->loc+flgblkcnt)*blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*blksz);
	*/
	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}
	
		
	
	return 0;
}
