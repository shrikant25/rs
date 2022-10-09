#include "vdheader.h"
#include "vdsyslib.h"
#include <string.h>
#include <stdio.h>


unsigned int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int *flbegloc){
	
	int i, j;
	FL_METADATA flmtd;
	unsigned int fd;
	unsigned int mtdata_loc = DSKINF.ttlmetadata_blks*sizeof(FL_METADATA);
	unsigned int mtdata_block = mtdata_loc / DSKINF.blksz;
 	unsigned int mtdata_loc_in_bloc = mtdata_loc - (mtdata_block * DSKINF.blksz);
	char *chptr;

	strcpy(flmtd.flnm, usrflnm);
	//strcat(flmtd.flnm, "\0");
	flmtd.isavailable = 1;
	flmtd.strtloc = flbegloc[0];
	flmtd.flsz = usrflsz;
	mtdata_block = (DSKINF.blkcnt - mtdata_block) - 1;
	fd = open(DSKINF.diskname, O_RDWR);
	memset(buffer, '\0', DSKINF.blksz);
	vdread(fd, buffer, mtdata_block, DSKINF.blksz);

	chptr = (char *)&flmtd;
	j = mtdata_loc_in_bloc;
	for(i = 0; i < sizeof(FL_METADATA); i++){
		buffer[j+i] = *chptr++;
	}

	if(fd){
	
		printf("writing metadata in %u at %d\n", mtdata_block, j);
		vdwrite(fd, buffer, mtdata_block, DSKINF.blksz);

		printf("filstatr loc %u\n", flmtd.strtloc);
		printf("filsz %u\n", flmtd.flsz);
		DSKINF.ttlmetadata_blks += 1;
		
		memset(buffer, '\0', DSKINF.blksz);
		vdread(fd, buffer, 0, DSKINF.blksz);
		
		chptr = (char *)&DSKINF.ttlmetadata_blks;
		printf("size is %d\n", buffer[0]);
		printf("size is %d\n", buffer[1]);
		buffer[2] = *chptr++;
		buffer[3] = *chptr++;
		buffer[4] = *chptr++;
		buffer[5] = *chptr;

		vdwrite(fd, buffer, 0, DSKINF.blksz);
		printf("total metadata blocks in the end %d\n", DSKINF.ttlmetadata_blks);
		close(fd);
		
	}
	else
		return -1;
	
	return 0;
}


int write_to_file(char *usrflnm,  int *beg_loc,unsigned long int byte_cnt){
	
	
	//setbits(beg_loc-(DSKINF.flgblkcnt*DSKINF.blksz), byte_cnt, 0);
	unsigned int urfl_fd = open(usrflnm, O_RDONLY);
	unsigned int disk_fd = open(DSKINF.diskname, O_RDWR);
	unsigned int bytes_to_write = 0;
	unsigned long int flbyte_cnt = byte_cnt;
	int i = 0;

	if(!(urfl_fd && disk_fd)) return -1;

	//lseek(disk_fd, beg_loc-1, SEEK_SET);
	printf("begloc %d and bytecnt %ld\n", beg_loc[0], flbyte_cnt);
	int b = beg_loc[i];
	while(flbyte_cnt>0){

		bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;				
		setbits(b, bytes_to_write, 0);
		memset(buffer, '\0', DSKINF.blksz);
		read(urfl_fd, buffer, bytes_to_write);
		//write(disk_fd, data_buf, bytes_to_write);
			printf("writing at 0 indexed block %d\n", b);
		vdwrite(disk_fd, buffer, b, DSKINF.blksz);
		flbyte_cnt -= bytes_to_write;
		printf("%d bytes written \n", bytes_to_write);
		b++;
	}
				
	close(disk_fd);
	close(urfl_fd);	
	
	write_metadata(usrflnm, byte_cnt, beg_loc);
	
	return 0;
}

