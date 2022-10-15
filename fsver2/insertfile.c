#include "vdheader.h"
#include "vdsyslib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int *get_emptmtdblk_loc(int fd, unsigned int *blk, unsigned int *loc_in_blk){

	unsigned int i,j = 0;	
	unsigned int mtdata_blocks = DSKINF.blksz/sizeof(FL_METADATA);
	unsigned int curblock = (DSKINF.dsksz/DSKINF.blksz)-1;
	FL_METADATA *flmtptr = NULL;
	// before taking value of curblock check wether its bit is set or not
	if(DSKINF.ttlmetadata_blks == 0){
		*blk = curblock;
		*loc_in_blk = 0;
		return 0;
	}

	i = 1;
	while(i<=DSKINF.ttlmetadata_blks){

		memset(buffer, '\0', DSKINF.blksz);
		vdread(fd, buffer, curblock, DSKINF.blksz);
		flmtptr = (FL_METADATA *)buffer;

		while(j<mtdata_blocks && i<=DSKINF.ttlmetadata_blks){
			if(!flmtptr->isavailable){	
				*blk = curblock;
				*loc_in_blk = j;
				return 0;
			}
			j++;
			i++;
		}
		
		if(j>=mtdata_blocks){
			curblock--;		
			j = 0;
		}	
	}

	if(i > DSKINF.ttlmetadata_blks){
		*blk = curblock;
		*loc_in_blk = j;
		return 0;
	}


	return NULL;
}


int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int flbegloc){
	
	FL_METADATA flmtd;
	FL_METADATA *flmtptr = NULL;
	unsigned int fd  = 0;
	char *chptr = NULL;
	int i = 1;
	int j = 0;
	unsigned int blk, loc_in_blk;
	int found = 0;

	fd = open(DSKINF.diskname, O_RDWR);
	
	if(fd){

		found = get_emptmtdblk_loc(fd, &blk, &loc_in_blk);

		if(found){	

			memset(buffer, '\0', DSKINF.blksz);
			strcpy(flmtd.flnm, usrflnm);
			flmtd.isavailable = 1;
			flmtd.strtloc = flbegloc;
			flmtd.flsz = usrflsz;

			chptr = (char *)&flmtd;
			for(i = 0; i < sizeof(FL_METADATA); i++){
				buffer[loc_in_blk] = *chptr++;
			}

			vdwrite(fd, buffer, blk, DSKINF.blksz);
			setbits(blk, 1, 0);

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

	unsigned int filedata_blocks = ceil((float)usrflsz/(float)DSKINF.blksz);
	unsigned int block_int_capacity = (DSKINF.blksz/sizeof(int)) - 1;
	unsigned int blocksdata_blocks = ceil((float)filedata_blocks/(float)block_int_capacity);
	printf("user file size %lu\n", usrflsz);
	printf("filedata blocks %u\n", filedata_blocks);
	printf("blockdata bloicks %u", blocksdata_blocks);
	unsigned int total_blocks_required = filedata_blocks + blocksdata_blocks;
	unsigned int *blocks = malloc(sizeof(int) * total_blocks_required);
	int status = getempty_blocks(total_blocks_required, blocks);

	for(int p = 0; p<total_blocks_required; p++)
		printf("i %d : b is %d\n", p, blocks[p]);

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

		temp = (k < filedata_blocks-1) ? blocks[i+1] : 0;
		chptr = (char *)&temp;

		buffer[j++] = *chptr++;
		buffer[j++] = *chptr++;
		buffer[j++] = *chptr++;
		buffer[j] = *chptr;
		printf("writing at blockl %d\n", blocks[i]);
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz);
		i++;
	}
	

	//lseek(disk_fd, blocks-1, SEEK_SET);
	//printf("begloc %d and bytecnt %ld\n", blocks[i], flbyte_cnt);
	
	//while(flbyte_cnt>0){
	printf("blsk re%u\n", total_blocks_required);
	int u = 0;
	while(u < filedata_blocks-1){

		//bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;				
		//setbits(blocks[i], bytes_to_write, 0);
		memset(buffer, '\0', DSKINF.blksz);
		read(usrfl_fd, buffer, DSKINF.blksz);
		
		//printf("writing at 0 indexed block %d\n", blocks[i]);
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz);
		printf("writing at blockl %d\n", blocks[i]);
		//printf("%d -- %s\n",  blocks[i],buffer);
		//flbyte_cnt -= bytes_to_write;
		//printf("%d bytes written \n", bytes_to_write);
		i++;
		u++;
		printf("u is %d iledata_blocks %d\n", u, filedata_blocks);
	}
	//}
				
	close(disk_fd);
	close(usrfl_fd);	
	printf("astal la vista baby");
	write_metadata(usrflnm, usrflsz, blocks[0]);
	printf("bruhhh\n");
	setbits(blocks, total_blocks_required, 0);
	return 0;
}

