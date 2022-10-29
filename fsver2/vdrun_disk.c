#include "vdsyslib.h"
#include "vdrun_disk.h"
#include "vdconstants.h"
#include "vddiskinfo.h"
#include "vddriver.h"
#include "vdwrite_to_buffer.h"
#include "vdwrite_flags_to_disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>




void readflags(int fd, unsigned long int *flags, DISKINFO DSKINF){
	
	int j = 0;
	int i = 0;
	int dataread = 0;
	unsigned long *ptr;
	char *buffer = malloc(DSKINF.blkcnt);
	int total_data = DSKINF.flags_arrsz * VDQUAD;

	i = 1;
	while(total_data > 0){
	
		dataread = vdread(fd, buffer, i, DSKINF.blksz);
		i++;
		total_data -= dataread;
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
	FR_FLGBLK_LST FFLST;
	FFLST.frblkcnt = 0;
	FFLST.head = NULL;
	fd = open(argv[1], O_RDONLY, 00777);
	
	if(fd){
	
		read(fd, &DSKINF, sizeof(DSKINF));
		printf("blksz %ld\n", DSKINF.blksz);
		printf("blcknt %ld\n", DSKINF.blkcnt);
		printf("flags arrasz %d\n", DSKINF.flags_arrsz);
		printf("flagsblkcnt %d\n", DSKINF.flgblkcnt);
		printf("ttlmtdta_blks %d\n", DSKINF.ttlmtdta_blks);
		printf(" DSKINF.dsk_blk_for_mtdata %d\n",  DSKINF.dsk_blk_for_mtdata);
		printf(" DSKINF.mtdta_blk_ofst %d\n",  DSKINF.mtdta_blk_ofst);
		printf("flag blocks count : %d\n", DSKINF.flgblkcnt);
		printf("total metadata blocks  %u\n", DSKINF.ttlmtdta_blks);
		printf("dsk blocks required for metadata blocks %d\n", DSKINF.ttlmtdta_blks);

		buffer = malloc(DSKINF.blksz);	
		flags = malloc(DSKINF.flags_arrsz * VDQUAD);
		readflags(fd, flags, DSKINF);
		

		build(DSKINF, flags, &FFLST);		
		
		printf("total blcks %d\n", FFLST.frblkcnt);
		printf("largest available block is at %d\n", FFLST.head->loc);
		printf("largest available block is at %ld\n", ((FFLST.head->loc)*DSKINF.blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*DSKINF.blksz);
		
		int filestatus = insert_file(fd, "b.txt", DSKINF, flags, &FFLST);

		if(filestatus != 0){
			printf("filestatus %d\n", filestatus);
			//perror("Failed to write file to disk");
			exit(EXIT_FAILURE);
		}
		else{   
			printf("agdain");// update flag values inside the disk
			if(write_flags_todisk(fd, flags, DSKINF) == -1){
				perror("Failed to write flags\n");
				exit(EXIT_FAILURE);
			}
		}
			
		free(buffer);
		free(flags);
		close(fd);
	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
