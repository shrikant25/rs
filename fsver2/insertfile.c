#include "vdheader.h"
#include "vdsyslib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int get_emtmtdblk_loc(int fd, unsigned int *blk, unsigned int *loc_in_blk){

	unsigned int i,j;	
	unsigned int mtdata_blocks = DSKINF.blksz/sizeof(FL_METADATA);
	unsigned int curblock = (DSKINF.dsksz/DSKINF.blksz)-1;
	FL_METADATA *flmtptr = NULL;
	
	// if there is no metadata in disk, then just add the metadata at last block
	if(DSKINF.ttlmetadata_blks == 0){
		// before taking value of curblock check wether its bit is set or not
		if(checkbits(curblock)){
			*blk = curblock;
			*loc_in_blk = 0;
			return 0;
		}
		return -1;
	}

	i = 1;
	j = 0;
	// if there is metadata already available in disk, 
	// then try to find a spot for new metadata
	// just search for a spot which was previously occupied
	// but now its free
	while(i<=DSKINF.ttlmetadata_blks){ // loop untill all blocks 

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
	
	// if current disk block is read then just go back one block 
		if(j>=mtdata_blocks){
				curblock--;		
				j = 0;
		}
			
	}

	// if all metadata block are read and
	// none of the previously used block is available
	// then just get a new pos

	if(i > DSKINF.ttlmetadata_blks){
		// is new fileblock and j = 0
		// it means the block doesnt have any metadata
		// which means it might be used for some other purpose
		// so need to check wether it is free or not
		if(j==0 && (!checkbits(curblock)))
			return -1;	
		
		*blk = curblock;
		*loc_in_blk = j;
		
		return 0;
	}

	return -1;
}


int write_metadata(char *usrflnm, unsigned int usrflsz, unsigned int flbegloc, int disk_fd, unsigned int blk, unsigned int loc_in_blk){
	
	// structure variable to store filematadat
	FL_METADATA flmtd;

	char *chptr = NULL;
	int i = 1;
	int j = 0;

	memset(buffer, '\0', DSKINF.blksz);
	vdread(disk_fd, buffer, blk, DSKINF.blksz);
	//store filename in structure variable 
	strcpy(flmtd.flnm, usrflnm);
	// set the metadata block as available
	// when the block is set as available 
	// it is considered as a valid block
	// if it is set to unavailable 
	// then it can be used to store data of some other file
	flmtd.strtloc = flbegloc;
	flmtd.flsz = usrflsz;

	//store bytes of structure variable in buffer at given location
	chptr = (char *)&flmtd;
	for(i = 0; i < sizeof(FL_METADATA); i++){
		buffer[loc_in_blk] = *chptr++;
	}

	// write metadata to disk
	vdwrite(disk_fd, buffer, blk, DSKINF.blksz);
	// set the bits as occupied for the block where metadata is written  
	setbits(blk, 1, 0);

	printf("filstatr loc %u\n", flmtd.strtloc);
	printf("filsz %u\n", flmtd.flsz);
	// increase the count of total metadatablocks that have been sotred
	DSKINF.ttlmetadata_blks += 1;

	memset(buffer, '\0', DSKINF.blksz);
	// read zeroth block
	vdread(disk_fd, buffer, 0, DSKINF.blksz);

	// write every byte of metadata to buffer at appopriate place
	chptr = (char *)&DSKINF.ttlmetadata_blks;
	printf("size is %d\n", buffer[0]);
	printf("size is %d\n", buffer[1]);
	buffer[2] = *chptr++;
	buffer[3] = *chptr++;
	buffer[4] = *chptr++;
	buffer[5] = *chptr;

	// write buffer to disk
	vdwrite(disk_fd, buffer, 0, DSKINF.blksz);
	printf("total metadata blocks in the end %d\n", DSKINF.ttlmetadata_blks);

	return 0;
}


int insert_file(char *usrflnm){
	
	printf("in function\n");
	// open user file
	// return if failed found
	int usrfl_fd = open(usrflnm, O_RDONLY, 00777);
	if(usrfl_fd == -1) return -1;
	printf("beyound\n");

	// get userfile size
	unsigned long int usrflsz = lseek(usrfl_fd, 0, SEEK_END);
	lseek(usrfl_fd, 0, SEEK_SET);

	// count how many  blocks will be required for file data to be stored
	unsigned int filedata_blocks = ceil((float)usrflsz/(float)DSKINF.blksz);
	
	// count how many intergers can be stored in one block 
	// here integer represents the block number of each block used to store the data of file
	unsigned int block_int_capacity = (DSKINF.blksz/sizeof(int)) - 1;
	
	// count how many blocks will be required to store all integers
	unsigned int blocksdata_blocks = ceil((float)filedata_blocks/(float)block_int_capacity);
	
	printf("user file size %lu\n", usrflsz);
	printf("filedata blocks %u\n", filedata_blocks);
	printf("blockdata bloicks %u", blocksdata_blocks);
	
	// total blocks required to store all the data
	unsigned int total_blocks_required = filedata_blocks + blocksdata_blocks;
	
	// createa an array to store all integers
	unsigned int *blocks = malloc(sizeof(unsigned int) * total_blocks_required);
	int status, p;
	unsigned int blk; 
	unsigned int loc_in_blk;
	int disk_fd;
	unsigned int bytes_to_write;
	char *chptr = NULL;
	int temp, i, j, k, x, u;
	int filedata_blocks_ints;

	//open disk, return if failed 
	disk_fd = open(DSKINF.diskname, O_RDWR);
	if(disk_fd == -1) return -1;
	
	// get required empty blocks
	// incase of failure return 
	status = getempty_blocks(total_blocks_required, blocks);
	if(status == -1)
		return -1;

	// get a location to store metadata
	// failure to get metadata means, disk is full or doesnt have enough space to store metadata
	// in any case of failure return
	status = get_emtmtdblk_loc(disk_fd, &blk, &loc_in_blk);
	if(status == -1)
		return -1;

	for(int p = 0; p<total_blocks_required; p++)
		printf("i %d : b is %d\n", p, blocks[p]);

	printf("got empty blocks\n");

	// just a variant to be used in loop
	filedata_blocks_ints = filedata_blocks;
	k = blocksdata_blocks;

	// loop untill all integers(that represent the number of blocks storing the data) are written on disk
	i = 0;
	while(filedata_blocks_ints > 0){

		memset(buffer, '\0', DSKINF.blksz);
		j = 0;
		x = 0;
		temp = 0;

		while(x<block_int_capacity){ // each block can hold only a certain amoutn of integers
		 
		 	temp = blocks[k++];		 // store kth integers in temp
			chptr = (char *)&temp;
			buffer[j++] = *chptr++;  // write each byte of integer at jth positon in buffer
			buffer[j++] = *chptr++;
			buffer[j++] = *chptr++;
			buffer[j++] = *chptr++;  
			x++;					 // increment after every integer is written in buffer(keeps track of integers written in buffer)
			filedata_blocks_ints--;	 // decrement after every integer is written (keeps track of overall integers that are been written)
		
			if(filedata_blocks_ints <= 0) break; // if all integers are written then break
		}

		// in every block that sotres integers, at last position there will be number of next block where data is stored
		// when there are no more integers to be written, just write zero indicating the end of set of integers
		temp = (k < filedata_blocks-1) ? blocks[i+1] : 0;
		chptr = (char *)&temp;   			
		buffer[j++] = *chptr++;     // write the bytes of integers to buffer
		buffer[j++] = *chptr++;
		buffer[j++] = *chptr++;
		buffer[j] = *chptr;
		printf("writing at blockl %d\n", blocks[i]);
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz); // write the block to file
		i++;		// increment to write integers in next block
	}
	

	
	printf("blsk re%u\n", total_blocks_required);
	u = 0;
	// write actual file data 
	while(u < filedata_blocks){

		
		memset(buffer, '\0', DSKINF.blksz);
		// read the data from userfile (size of data to be read = size of block in disk)
		read(usrfl_fd, buffer, DSKINF.blksz); 
		
		// write data to disk at ith block
		vdwrite(disk_fd, buffer, blocks[i], DSKINF.blksz);
		printf("writing at blockl %d\n", blocks[i]);
		
		i++; // increment to get next block
		u++; // keeps track of data blocks that have been written
		printf("u is %d iledata_blocks %d\n", u, filedata_blocks);
	}
				
	//close userfile 
	close(usrfl_fd);	
	printf("astal la vista baby");

	// write metadata
	// the function accetps userfile name, userfile size, value of first block storing the integers
	// file descriptor for disk, pointer to variable that can store the block number where metadata will be written
	// and pointer to a variable that will store the location inside the block where metadata will be written
	write_metadata(usrflnm, usrflsz, blocks[0], disk_fd, blk, loc_in_blk);
	printf("bruhhh\n");

	//for all the blocks that have been written, set the value of bits representing those blocks to occupied 
	setbits(blocks, total_blocks_required, 0); // zero represents occoupied

	//close disk
	close(disk_fd);
	return 0;
}

