#include "vdsyslib.h"
#include "vdheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void display();
void display_flags();
void insert();
FR_FLGBLK * createblk(unsigned int);
void insertblk(FR_FLGBLK *);
void build(int);
int write_to_file(char *, int *, unsigned long int);
int write_flags_todisk(unsigned int);
FR_FLGBLK_LST FFLST;
unsigned long int *flags;
char *buffer;
DISKINFO DSKINF;

/*
void display_flags(){
	
	unsigned int i;
	for(i = 0; i<chunks; i++) // chunks = ((disksize/blocksize)-512)/64 ,,,,,, ((2^32/ 2^10)-512)/64 = 65528 
		printf("%d : %lx\n", i, flags[i]);

}
*/


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


int write_flags_todisk(unsigned int flg_datasz){
	
	char *chptr;
	unsigned int fd = open(DSKINF.diskname, O_WRONLY);
	flg_datasz /= 8;
	int i, j, k;
	printf("\nflags  %lx\n", flags[6]);
	printf("\nflags  %lx\n", flags[7]);
	printf("\nflags  %lx\n", flags[8]);
	printf("\n%u\n", flg_datasz);	
	if(fd){
		i = 1;
		k = 0;
		while(k<flg_datasz){

			memset(buffer, '\0', DSKINF.blksz);
			j = 0;
//			printf("here");
			while(j<DSKINF.blksz){
				chptr = (char *)&(flags[k]);
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr;
				k++;
			}
			
			vdwrite(fd, buffer, i, DSKINF.blksz);
			i++;

		}
	printf("doe");
		close(fd);
		return 0;
	}
	return -1;
}	


int readflags(int fd, int readsize){
	
	int j = 0;
	int i = 1;
	int dataread = 0;
	int total_dataread = 0;
	unsigned long *ptr;
	
	while(readsize > 0){
	
		dataread = vdread(fd, buffer, i, DSKINF.blksz);
		i++;
		total_dataread += dataread;
		readsize -= dataread;

		ptr = (unsigned long *)buffer;
		while(dataread > 0){
			flags[j++] = *ptr++;
			dataread -= (ULBCNT/8);
		}

	}
	return total_dataread;
}


int main(int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}
	
	DSKINF.diskname = argv[1];
	char ch;
	unsigned int fd = open(DSKINF.diskname, O_RDONLY);
	unsigned int rdcnt;
	unsigned int readsize;
	int chunks;
	unsigned long int usrflsz;
	FFLST.frblkcnt = 0;
	FFLST.head = NULL;
	
	if(fd){
		read(fd, &ch, sizeof(char));
		DSKINF.dsksz = (unsigned long int)pow(2, ch);

		read(fd, &ch, sizeof(char));
		DSKINF.blksz = (unsigned long int)pow(2, ch);

		buffer = calloc(sizeof(char), DSKINF.blksz);	
		vdread(fd, buffer, 0, DSKINF.blksz); 
		
		DSKINF.ttlmetadata_blks = *((int *)&buffer[2]);
			
		DSKINF.blkcnt = DSKINF.dsksz/DSKINF.blksz;
		DSKINF.flgblkcnt = (DSKINF.blkcnt/8) / DSKINF.blksz; //512

		readsize = DSKINF.blkcnt / 8;	
		printf("\nreadsize is %d\n", readsize);
		flags = malloc(readsize);
		rdcnt = readflags(fd, readsize);

		printf("total metadata blocks in the begining %u\n", DSKINF.ttlmetadata_blks);
		close(fd);
	
		chunks = DSKINF.blkcnt / ULBCNT;
	
		if(rdcnt == readsize)
			build(chunks);
		else{
			perror("Failed to read flags\n");
			exit(EXIT_FAILURE);
		}
		
		
		printf("total blcks %d\n", FFLST.frblkcnt);
		
		printf("largest available block is at %d\n", FFLST.head->loc);
		printf("largest available block is at %ld\n", ((FFLST.head->loc)*DSKINF.blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*DSKINF.blksz);
		
		fd = open("a.txt", O_RDONLY);
		if(fd){
			usrflsz = lseek(fd, 0, SEEK_END);

			//unsigned int blocks_required = ceil(usrflsz/DSKINF.blksz);
			//unsigned int blockptr_blocks_required = ceil((blocks_required * sizeof(int))/DSKINF.blksz);

			close(fd);
			int a[10];
			
			a[0] = FFLST.head->loc;
			printf(" a0 %d\n", a[0]);
			if(write_to_file("a.txt", a, usrflsz) == -1){
				perror("Failed to write file to disk");
				exit(EXIT_FAILURE);
			}
			else{   // update flag values inside the disk
				if(write_flags_todisk(readsize) == -1){
					perror("Failed to write flags\n");
					exit(EXIT_FAILURE);
				}
			}
		
		}
		else{
			printf("Failed to open file\n");
		}
		
	//	display_flags();
		
		write_flags_todisk(readsize); // readsize = flag data size		
	
	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

		
	
	return 0;
}
