#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<math.h>

#define ULBCNT  sizeof(unsigned long int)*8 // number of bits in unsigned long


typedef struct FR_FLGBLK{
	unsigned int loc;
	unsigned int cnt;
	struct FR_FLGBLK *next;
}FR_FLGBLK;


typedef struct{
	FR_FLGBLK *head;
	int frblkcnt;
}FR_FLGBLK_LST;


FR_FLGBLK_LST FFLST;
unsigned long int dsksz;
unsigned long int blksz;
unsigned long int *flags;
int blkcnt;
int flgblkcnt;


void display();
void insert();
FR_FLGBLK * createblk(int);
void insertblk(FR_FLGBLK *);
void build(int);

void display(){

	FR_FLGBLK *temp = FFLST.head;
	while(temp){
		printf("location :%d \n", temp->loc);
		printf("cnt : %d\n", temp->cnt);
		temp = temp->next;
		printf("\n\n");
	}
	printf("total blcks %d\n", FFLST.frblkcnt);
}


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


FR_FLGBLK * createblk(int bitloc){

	FR_FLGBLK * blk = (FR_FLGBLK *)malloc(sizeof(FR_FLGBLK));
	blk->loc = (unsigned int)bitloc;
	blk->cnt = 0;

	return blk;
}


void build(int chunk){

	int i,j;
	unsigned long int temp = 0;
	FR_FLGBLK *fr_flgblk = NULL;

	for(i=0; i<chunk; i++){

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


int main(int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}

	unsigned char ch;
	int fd = open(argv[1], O_RDONLY);
	int rdcnt;
	int readsize;

	if(fd){
		printf("opneed");
		read(fd, &ch, sizeof(char));
		dsksz = (long int)pow(2, ch);

		read(fd, &ch, sizeof(char));
		blksz = (long int)pow(2, ch);
	
		blkcnt = dsksz/blksz;
		flgblkcnt = (blkcnt/8) / blksz; 
	
		readsize = flgblkcnt * blksz;	
		readsize -= (flgblkcnt/(ULBCNT/8));
	
		flags = malloc(readsize);
		rdcnt = read(fd, flags, readsize);
		close(fd);

		flags[22]    =  0xFFFFFFF0FFFFFFF0;
		flags[21]    =  0x0FFFFFF000000000;
		flags[30000] =  0x0FFFFFF0FFFF0FF0;
		flags[64000] =  0xFFFFFFF0FFFFFFFF;
		flags[3121]  =  0xFFFFFFF0FFFFFFFF;
		flags[512]   =  0xFFF00FF0FFFFFFFF;
		flags[1000]  =  0xFF000FFF0FFFFFFF;
		flags[1]     =  0x0000000000000000;
		flags[3111]  =  0xFFF00FF0FFFFFF0F;

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


		if(rdcnt == readsize)
			build(readsize/(ULBCNT/8));
		else{
			perror("Failed to read flags\n");
			exit(EXIT_FAILURE);
		}

	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	printf("total blcks %d\n", FFLST.frblkcnt);
	
	printf("largest available block is at %d\n", FFLST.head->loc+flgblkcnt+1);
	printf("largest available block is at %ld\n", ((FFLST.head->loc+flgblkcnt)*blksz)+1);
	printf("total empty  blocks %d\n", FFLST.head->cnt);
	printf("total empty  bytes %ld\n", (FFLST.head->cnt)*blksz);
//	display();

	return 0;
}
