#include "vdsyslib.h"
#include "vdrun_disk.h"
#include "vdconstants.h"
#include "vddiskinfo.h"
#include "vdwrite_to_buffer.h"
#include "vdwrite_flags_to_disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

FR_FLGBLK_LST FFLST;

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


void readflags(int fd, unsigned long int *flags, DISKINFO DSKINF){
	
	int j = 0;
	int i = 1;
	int dataread = 0;
	unsigned long *ptr;
	char *buffer = malloc(DSKINF.blkcnt);

	while(DSKINF.flags_arrsz > 0){
	
		dataread = vdread(fd, buffer, i, DSKINF.blksz);
		i++;
		DSKINF.flags_arrsz -= dataread;

		ptr = (unsigned long *)buffer;
		while(dataread > 0){
			flags[j++] = *ptr++;
			dataread -= VDQUAD;
		}

	}
	
	free(buffer);

}


int main(int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}
	
	char *buffer;
	unsigned int fd = 0; 
	unsigned long int *flags;
	DISKINFO DSKINF;
	FFLST.frblkcnt = 0;
	FFLST.head = NULL;

	fd = open(argv[1], O_RDONLY);
	
	if(fd){
	
		read(fd, &DSKINF, sizeof(DSKINF));
	
		buffer = malloc(DSKINF.blksz);	
		flags = malloc(DSKINF.flags_arrsz);
		readflags(fd, flags, DSKINF);
		
		close(fd);

		build();		
		
		printf("total blcks %d\n", FFLST.frblkcnt);
		printf("largest available block is at %d\n", FFLST.head->loc);
		printf("largest available block is at %ld\n", ((FFLST.head->loc)*DSKINF.blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*DSKINF.blksz);
		
		int filestatus = insert_file("b.txt");

		if(filestatus == -1){
			perror("Failed to write file to disk");
			exit(EXIT_FAILURE);
		}
		else{   // update flag values inside the disk
			if(write_flags_todisk(flags, DSKINF) == -1){
				perror("Failed to write flags\n");
				exit(EXIT_FAILURE);
			}
		}
		
		write_flags_todisk(flags, DSKINF); // readsize = flag data size		
		free(buffer);
		free(flags);
	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
