#include "vdconstants.h"
#include "vdrun_disk.h"
#include "vddiskinfo.h"
#include "vdsyslib.h"
#include "vdwrite_to_buffer.h"
#include "vdfile_metadata.h"
#include "vddriver.h"
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

typedef struct DATA_LEVEL{
	unsigned int block_cnt;
	struct DATA_LEVEL *next;
}DATA_LEVEL;

DATA_LEVEL * new_val_node(unsigned int val){
	DATA_LEVEL *new = malloc(sizeof(DATA_LEVEL));
	new->next = NULL;
	new->block_cnt = val;
	return new;
}

void insert_val(unsigned int val, DATA_LEVEL **head){
	DATA_LEVEL *new = new_val_node(val);
	new->next = *head;
	*head = new; 
}


int insert_file(char *usrflnm, DISKINFO DSKINF){
	
	printf("in function\n");
	// open user file
	// return if failed found
	int usrfl_fd = open(usrflnm, O_RDONLY, 00777);
	unsigned int val = 0;
	unsigned int size;
	unsigned int *blocks1 = NULL;
	unsigned int *blocks2 = NULL;
	unsigned int *temp_block = NULL;
	int disk_fd;
	char *buffer = malloc(sizeof(char) * DSKINF.blksz);
	int i = 0;
	if(usrfl_fd == -1) return -1;
	printf("beyound\n");

	// get userfile size
	unsigned long int usrflsz = lseek(usrfl_fd, 0, SEEK_END);
	lseek(usrfl_fd, 0, SEEK_SET);

	// count how many  blocks will be required for file data to be stored
	unsigned int filedata_blocks = ceil((float)usrflsz/(float)DSKINF.blksz);

	// count how many intergers can be stored in one block 
	// here integer represents the block number of each block used to store the data of file
	unsigned int block_int_capacity = (DSKINF.blksz/sizeof(int));
	val = filedata_blocks/block_int_capacity;
	DATA_LEVEL *head;
	insert_val(val, &head);

	while(val != 1){
		val = val/block_int_capacity;
		insert_val(val, &head);
	}

	blocks1 = malloc(sizeof(int) * block_int_capacity);
	blocks2 = malloc(sizeof(int) * block_int_capacity);
	DATA_LEVEL *level = head;
	getempty_blocks(level->block_cnt, blocks1);
	int beg_loc = blocks1[0];
	DATA_LEVEL *data = head->next;
		
	//open disk, return if failed 
	disk_fd = open(DSKINF.diskname, O_RDWR);
	if(disk_fd == -1) return -1;

	while(data != NULL){

		int temp = data->block_cnt;
		size = 0;

		for(i = 0; i<level->block_cnt; i++){
			
			size = temp > block_int_capacity ? block_int_capacity : temp;
			getempty_blocks(size, blocks2);
			
			memset(buffer, 0, DSKINF.blksz);
			write_to_buffer(buffer, (char *)blocks2, size*VDDOUBLE_WORD, 0);
			vdwrite(disk_fd, buffer, blocks1[i], DSKINF.blksz);
			
			temp -= size;
		}

		level = level->next;
		temp_block = blocks1;
		blocks1 = blocks2;
		blocks2 = blocks1;
		data = level->next;

	}

	int status, p;
	unsigned int blk; 
	unsigned int loc_in_blk;
	int disk_fd;
	unsigned int bytes_to_write;
	char *chptr = NULL;
	int temp, i, j, k, x, u;
	int filedata_blocks_ints;

	
	// get required empty blocks
	// get a location to store metadata
	// failure to get metadata means, disk is full or doesnt have enough space to store metadata
	// in any case of failure return
	status = get_emtmtdblk_loc(disk_fd, &blk, &loc_in_blk);
	if(status == -1)
		return -1;


	// just a variant to be used in loop
	filedata_blocks_ints = filedata_blocks;
	k = blocksdata_blocks;


	
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

