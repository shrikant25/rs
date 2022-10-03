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
int write_to_file(char *, unsigned int, unsigned long int);
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
	
	unsigned int data_written = 0;
	unsigned int fd = open(DSKINF.diskname, O_WRONLY);
	
	lseek(fd, 2, SEEK_SET);
	data_written = write(fd, flags, flg_datasz);
	close(fd);
	if(data_written != flg_datasz)
		return -1;
	return 0;
	
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

		readsize = DSKINF.flgblkcnt / (ULBCNT);	
	
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
		
		printf("largest available block is at %d\n", FFLST.head->loc+DSKINF.flgblkcnt+1);
		printf("largest available block is at %ld\n", ((FFLST.head->loc+DSKINF.flgblkcnt)*DSKINF.blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*DSKINF.blksz);
		
		fd = open("c.txt", O_RDONLY);
		if(fd){
			usrflsz = lseek(fd, 0, SEEK_END);

			//unsigned int blocks_required = ceil(usrflsz/DSKINF.blksz);
			//unsigned int blockptr_blocks_required = ceil((blocks_required * sizeof(int))/DSKINF.blksz);

			close(fd);
			if(write_to_file("c.txt", ((FFLST.head->loc+DSKINF.flgblkcnt +1)*DSKINF.blksz)+1, usrflsz) == -1){
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
