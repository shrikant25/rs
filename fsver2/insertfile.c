#include "vdheader.h"
#include "vdsyslib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int flbegloc){
	
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
	flmtd.strtloc = flbegloc;
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
		setbits(&mtdata_block, 1, 0);

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


//int write_to_file(char *usrflnm,  unsigned long int byte_cnt, unsigned int *blocks, int filedata_blocks, int blocksdata_blocks, unsigned int block_int_capacity){
int insert_file(char *usrflnm){
	
	printf("in function\n");
	unsigned int usrfl_fd = open(usrflnm, O_RDONLY, 00777);
	if(usrfl_fd == -1) return -1;
	printf("beyound\n");
	unsigned long int usrflsz = lseek(usrfl_fd, 0, SEEK_END);
	lseek(usrfl_fd, 0, SEEK_SET);

	unsigned int filedata_blocks = usrflsz/DSKINF.blksz;
	unsigned int block_int_capacity = (DSKINF.blksz/sizeof(int)) - 1;
	unsigned int blocksdata_blocks = ceil(filedata_blocks/block_int_capacity);
	unsigned int total_blocks_required = filedata_blocks + blocksdata_blocks;
	unsigned int *blocks = malloc(sizeof(int) * total_blocks_required);
	int status = getempty_blocks(total_blocks_required, blocks);

	if(status == -1)
		return status;

	printf("got empty blocks\n");
	unsigned int disk_fd = open(DSKINF.diskname, O_RDWR);
	if(disk_fd == -1) return -1;

	unsigned int bytes_to_write = 0;
	char *chptr = NULL;
	int temp, i, j, k, x;
	
	int filedata_blocks_ints = filedata_blocks;
	k = blocksdata_blocks;

	i = 0;
	while(filedata_blocks_ints > 0){

		memset(buffer, '\0', DSKINF.blksz);
		j = 0;
		x = 0;
		temp = 0;

		while(x<block_int_capacity){
		 
		 	temp = blocks[k++];
			chptr = (char *)&temp;
			buffer[j++] = *chptr++;
			buffer[j++] = *chptr++;
			buffer[j++] = *chptr++;
			buffer[j++] = *chptr++;
			x++;
			filedata_blocks_ints--;		
		
			if(filedata_blocks_ints <= 0) break;
		}

		temp = (k < blocksdata_blocks-1) ? k : -1;
		chptr = (char *)&temp;

		buffer[j++] = *chptr++;
		buffer[j++] = *chptr++;
		buffer[j++] = *chptr++;
		buffer[j] = *chptr;
	
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz);
		i++;
	}
	

	//lseek(disk_fd, blocks-1, SEEK_SET);
	//printf("begloc %d and bytecnt %ld\n", blocks[i], flbyte_cnt);
	
	//while(flbyte_cnt>0){
	
	while(i < total_blocks_required){

		//bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;				
		//setbits(blocks[i], bytes_to_write, 0);
		memset(buffer, '\0', DSKINF.blksz);
		read(usrfl_fd, buffer, DSKINF.blksz);
		
		printf("writing at 0 indexed block %d\n", blocks[i]);
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz);
		//flbyte_cnt -= bytes_to_write;
		//printf("%d bytes written \n", bytes_to_write);
		i++;
	}
	//}
				
	close(disk_fd);
	close(usrfl_fd);	

	write_metadata(usrflnm, usrflsz, blocks[0]);
	printf("bruhhh\n");
	setbits(blocks, total_blocks_required, 0);
	return 0;
}

