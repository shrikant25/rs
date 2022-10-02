#include "vdheader.h"
#include "vdsyslib.h"
#include <string.h>
#include <stdio.h>


unsigned int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int flbegloc){
	
	FL_METADATA flmtd;
	unsigned int mtdata_loc =  DSKINF.dsksz - (DSKINF.ttlmetadata_blks*sizeof(FL_METADATA));         			  // write metadata from this location 
	mtdata_loc -= sizeof(FL_METADATA);
	int mtdata_block = (mtdata_loc+1)/DSKINF.blksz;
	memset(buffer, '\0', DSKINF.blksz);
	strcpy(flmtd.flnm, usrflnm);
	flmtd.strtloc = flbegloc;
	flmtd.flsz = usrflsz;
	
	unsigned int fd = open(DSKINF.diskname, O_WRONLY);
	vdread(fd, buffer, mtdata_block, DSKINF.blksz);
	
	if(fd){
	
		//lseek(fd, mtdata_loc-1, SEEK_SET);

		printf("writing metadata at %u\n", mtdata_loc-1);
		vdwrite(fd, buffer);
	//	write(fd, &flmtd, sizeof(flmtd));     				        // write metadata to file
		printf("filstatr loc %u\n", flmtd.strtloc);
		printf("filsz %u\n", flmtd.flsz);
		DSKINF.ttlmetadata_blks += 1;
		lseek(fd, -4, SEEK_END);
		write(fd, &DSKINF.ttlmetadata_blks, sizeof(DSKINF.ttlmetadata_blks));
		printf("total metadata blocks in the end %d\n", DSKINF.ttlmetadata_blks);
		close(fd);
		
	}
	else
		return -1;
	
	return 0;
}


int write_to_file(char *usrflnm, unsigned int *beg_loc,unsigned long int byte_cnt){
	
	
	setbits(beg_loc-(DSKINF.flgblkcnt*DSKINF.blksz), byte_cnt, 0);
	unsigned int urfl_fd = open(usrflnm, O_RDONLY);
	unsigned int disk_fd = open(DSKINF.diskname, O_WRONLY);
	unsigned int bytes_to_write = 0;
	unsigned long int flbyte_cnt = byte_cnt;
	int i = 0;

	if(!(urfl_fd && disk_fd)) return -1;

	//lseek(disk_fd, beg_loc-1, SEEK_SET);
	printf("begloc %d and bytecnt %ld\n", beg_loc, flbyte_cnt);
	
	while(flbyte_cnt>0){

		bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;				
		memset(buffer, '\0', DSKINF.blksz);
		read(urfl_fd, buffer, bytes_to_write);
		//write(disk_fd, data_buf, bytes_to_write);
		vdwrite(disk_fd, buffer, beg_loc[i], DSKINF.blksz);
		flbyte_cnt -= bytes_to_write;
		printf("%d bytes written \n", bytes_to_write);
	}
				
	close(disk_fd);
	close(urfl_fd);	
	
	write_metadata(usrflnm, byte_cnt, beg_loc);
	
	return 0;
}

