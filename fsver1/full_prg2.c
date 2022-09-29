	#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<math.h>
#define BUFLEN 1024
#define ULBCNT sizeof(unsigned long int)*8 // number of bits in unsigned long


typedef struct FR_FLGBLK{
	unsigned int loc;
	unsigned int cnt;
	struct FR_FLGBLK *next;
}FR_FLGBLK;


typedef struct{
	FR_FLGBLK *head;
	unsigned int frblkcnt;
}FR_FLGBLK_LST;


typedef struct FL_METADATA{
	char flnm[64];
	unsigned int strtloc;
	unsigned int flsz;
}FL_METADATA;


FR_FLGBLK_LST FFLST;
unsigned long int dsksz;
unsigned long int blksz;
unsigned long int *flags;
unsigned int blkcnt;
unsigned int flgblkcnt;
unsigned int chunks;
const char * diskname;
unsigned int ttlmetadata_blks;


void display();
void display_flags();
void insert();
FR_FLGBLK * createblk(unsigned int);
void insertblk(FR_FLGBLK *);
void build();
int write_to_file(char *, unsigned int, unsigned long int);
int setbits(unsigned int, unsigned long int, unsigned int);
int write_flags_todisk(unsigned int);


unsigned int write_metdata(char *usrflnm, unsigned int usrflsz, unsigned int flbegloc){
	
	FL_METADATA flmtd;
	unsigned int mtdata_loc =  dsksz - (ttlmetadata_blks*sizeof(FL_METADATA)+4);         			  // write metadata from this location 
	mtdata_loc -= sizeof(FL_METADATA);
		
	
	strcpy(flmtd.flnm, usrflnm);
	flmtd.strtloc = flbegloc;
	flmtd.flsz = usrflsz;
	
	unsigned int fd = open(diskname, O_WRONLY);
	
	if(fd){
	
		lseek(fd, mtdata_loc-1, SEEK_SET);
		printf("writing metadata at %u\n", mtdata_loc-1);
		write(fd, &flmtd, sizeof(flmtd));     				        // write metadata to file
		printf("filstatr loc %u\n", flmtd.strtloc);
		printf("filsz %u\n", flmtd.flsz);
		ttlmetadata_blks += 1;
		lseek(fd, -5, SEEK_END);
		write(fd, &ttlmetadata_blks, sizeof(ttlmetadata_blks));
		printf("total metadata blocks in the end %d\n", ttlmetadata_blks);
		close(fd);
		
	}
	else
		return -1;
	
	return 0;
}

/*
void display_flags(){
	
	unsigned int i;
	for(i = 0; i<chunks; i++) // chunks = ((disksize/blocksize)-512)/64 ,,,,,, ((2^32/ 2^10)-512)/64 = 65528 
		printf("%d : %lx\n", i, flags[i]);

}
*/

int setbits(unsigned int beg_loc, unsigned long int bytes, unsigned int bitsign){ 
	
	
	unsigned int bitcnt = ceil((float)bytes/(float)blksz);
	unsigned int flag = 0;
	unsigned int startbit_inflag = 0;
	unsigned int loc = beg_loc;
	unsigned int num_bits_manp = 0;
	
	while(bitcnt){
	
		flag = ceil((float)loc/(float)64);
		startbit_inflag = loc - ((flag-1) * ULBCNT);
		loc -= startbit_inflag;
	
		if(startbit_inflag == 1 && bitcnt>=64){
			flags[flag-1] = 0xFFFFFFFFFFFFFFFF * bitsign;
			startbit_inflag = 65;
			bitcnt -= 64;
		}
		else{
	        	while(startbit_inflag<=64 && bitcnt>0){
				flags[flag-1] = (flags[flag-1] & ~(1UL << (startbit_inflag-1))) | (bitsign << (startbit_inflag-1)); 	
				bitcnt--;            
				startbit_inflag++;		
			}                                      
		}		
		       				       
		loc += startbit_inflag;
	}

}


int write_to_file(char *usrflnm, unsigned int beg_loc,unsigned long int byte_cnt){
	
	
	setbits(beg_loc-(flgblkcnt*blksz), byte_cnt, 0);
	char data_buf[BUFLEN];
	unsigned int urfl_fd = open(usrflnm, O_RDONLY);
	unsigned int disk_fd = open(diskname, O_WRONLY);
	unsigned int bytes_to_write = 0;
	unsigned long int flbyte_cnt = byte_cnt;

	if(!(urfl_fd && disk_fd)) return -1;

	lseek(disk_fd, beg_loc-1, SEEK_SET);
	printf("begloc %d and bytecnt %ld\n", beg_loc, flbyte_cnt);
	while(flbyte_cnt>0){

		bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;			
		read(urfl_fd, data_buf, bytes_to_write);
		write(disk_fd, data_buf, bytes_to_write);
		flbyte_cnt -= bytes_to_write;
		printf("%d bytes written \n", bytes_to_write);
	}
				
	close(disk_fd);
	close(urfl_fd);	
	
	write_metdata(usrflnm, byte_cnt, beg_loc);
	
	
	return 0;
}


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


void insertblk(FR_FLGBLK *newfr_blk){

	if(FFLST.head == NULL){
		FFLST.frblkcnt = 1;
		FFLST.head = newfr_blk;
	}
	else{
	
		FFLST.frblkcnt++;
		
		if(FFLST.head->cnt < newfr_blk->cnt){
			newfr_blk->next = FFLST.head;
			FFLST.head = newfr_blk;	
		}
		else{
			newfr_blk->next = FFLST.head->next;
			FFLST.head->next = newfr_blk;
		}
	}
}


FR_FLGBLK * createblk(unsigned int bitloc){

	FR_FLGBLK * blk = (FR_FLGBLK *)malloc(sizeof(FR_FLGBLK));
	blk->loc = (unsigned int)bitloc;
	blk->cnt = 0;

	return blk;
}


void build(){

	int i,j;
	unsigned long int temp = 0;
	FR_FLGBLK *fr_flgblk = NULL;

	for(i=0; i<chunks; i++){

		if(flags[i]){
			for(j=0; j<ULBCNT; j++){
					
				if((flags[i]>>j) & 1){
					
					if(!fr_flgblk){
						fr_flgblk = createblk((i*ULBCNT)+j);
						
					}
					fr_flgblk->cnt++;
				}
				else{ 
					if(fr_flgblk)
						insertblk(fr_flgblk);
					fr_flgblk = NULL;
				}
			}	
		}
		else{
			 
			if(fr_flgblk)
				insertblk(fr_flgblk);
			 fr_flgblk = NULL;
		}
	}	
	if(fr_flgblk)
		insertblk(fr_flgblk);	

}


int write_flags_todisk(unsigned int flg_datasz){
	
	unsigned int data_written = 0;
	unsigned int fd = open(diskname, O_WRONLY);
	
	lseek(fd, 2, SEEK_SET);
	data_written = write(fd, flags, flg_datasz);
	close(fd);
	if(data_written != flg_datasz)
		return -1;
	return 0;
	
}	



int main(unsigned int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}
	
	diskname = argv[1];
	char ch;
	unsigned int fd = open(diskname, O_RDONLY);
	unsigned int rdcnt;
	unsigned int readsize;
	unsigned long int usrflsz;

	if(fd){
		read(fd, &ch, sizeof(char));
		dsksz = (unsigned long int)pow(2, ch);

		read(fd, &ch, sizeof(char));
		blksz = (unsigned long int)pow(2, ch);
	
		blkcnt = dsksz/blksz;
		flgblkcnt = (blkcnt/8) / blksz; 
	
		readsize = flgblkcnt * blksz;	
		readsize -= (flgblkcnt/(ULBCNT/8));
		chunks = readsize/(ULBCNT/8);
	
		flags = malloc(readsize);
		rdcnt = read(fd, flags, readsize);
		
		lseek(fd, -5, SEEK_END);
		read(fd, &ttlmetadata_blks, sizeof(ttlmetadata_blks));
		printf("total metadata blocks in the begining %u\n", ttlmetadata_blks);
		close(fd);


	
		if(rdcnt == readsize)
			build();
		else{
			perror("Failed to read flags\n");
			exit(EXIT_FAILURE);
		}
		
		
		printf("total blcks %d\n", FFLST.frblkcnt);
		
		printf("largest available block is at %d\n", FFLST.head->loc+flgblkcnt+1);
		printf("largest available block is at %ld\n", ((FFLST.head->loc+flgblkcnt)*blksz)+1);
		printf("total empty  blocks %d\n", FFLST.head->cnt);
		printf("total empty  bytes %ld\n", (FFLST.head->cnt)*blksz);
		
		fd = open("c.txt", O_RDONLY);
		if(fd){
			usrflsz = lseek(fd, 0, SEEK_END);
			close(fd);
			if(write_to_file("c.txt", ((FFLST.head->loc+flgblkcnt)*blksz)+1, usrflsz) == -1){
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
		
	//	display_flags();
		
		write_flags_todisk(readsize); // readsize = flag data size		
	
	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

		
	
	return 0;
}
