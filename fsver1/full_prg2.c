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
	int frblkcnt;
}FR_FLGBLK_LST;


typedef struct FL_METADATA{
	unsigned char flnmsz;
	char *flnm;
	unsigned long int strtloc;
	unsigned long int flsz;
}FL_METADATA;


FR_FLGBLK_LST FFLST;
unsigned long int dsksz;
unsigned long int blksz;
unsigned long int *flags;
unsigned int blkcnt;
unsigned int flgblkcnt;
unsigned int chunks;
const char * diskname;
int ttlmetadata_sz;


void display();
void display_flags();
void insert();
FR_FLGBLK * createblk(int);
void insertblk(FR_FLGBLK *);
void build();
int write_to_file(char *, int, int);
int setbits(int, unsigned long int, int);
int write_flags_todisk(int);


int write_metdata(char *usrflnm, int usrflsz, int flbegloc){
	
	unsigned char flnmsz = strlen(usrflnm);
	char * chptr;	
	int mtdata_sz = sizeof(char) + strlen + (sizeof(int)*2);
	char *buf = malloc(mtdata_sz+1);
	int i = 0;
	int j = 0;
	int mtdata_loc =  dskz - ttlmetadata_sz;           // write metadata from this location 
	
	
	chptr = &usrflsz;                                 // add userfilename size to buffer (byte by byte)
	for(i=0; i<4; i++){
		buf[j++] = chptr[i];						  
	}
	
	chptr = &flgbegloc;								 // add beginning location of file's data in disk (byte by byte)
	for(i=0; i<4; i++){
		buf[j++] = chptr[i];
	}

	for(i=0; i<flnmsz; i++){						 // add filename to buffer
		buf[j++] = usrflnm[i];
	}
	
	chptr = &flnmsz;								 //	add filename's length to buffer
	buf[j++] =  *chptr;
	buf[j] = '\0';
	
	int fd = open(diskname, O_WRONLY);
	
	if(fd){
	
		lseek(fd, metadata_loc-(mtdata_sz-1), SEEK_SET);
		write(fd, buf, mtdata_sz);            // write metadata to file
		close(fd);
		
		
		ttlmetadata_sz += mtdata_sz;
		
		
	}
	else
		return -1;
	
	return 0;
}


void display_flags(){
	
	int i;
	for(i = 0; i<chunks; i++)
		printf("%d : %lx\n", i, flags[i]);

}


int setbits(int beg_loc, unsigned long int bytes, int bitsign){ 
	
	
	int bitcnt = ceil((float)bytes/(float)blksz);
	int flag = 0;
	int startbit_inflag = 0;
	int loc = beg_loc;
	int num_bits_manp = 0;
	
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


int write_to_file(char *usrflnm, int beg_loc, int byte_cnt){
	
	
	setbits(beg_loc-(flgblkcnt*blksz), byte_cnt, 0);
	char data_buf[BUFLEN];
	int urfl_fd = open(usrflnm, O_RDONLY);
	int disk_fd = open(diskname, O_WRONLY);
	int bytes_to_write = 0;
	int flbyte_cnt = byte_cnt;

	if(!(urfl_fd && disk_fd)) return -1;

	lseek(disk_fd, beg_loc-1, SEEK_SET);
	printf("begloc %d and bytecnt %d\n", beg_loc, flbyte_cnt);
	while(flbyte_cnt>0){

		bytes_to_write = flbyte_cnt > BUFLEN ? BUFLEN : flbyte_cnt;			
		read(urfl_fd, data_buf, bytes_to_write);
		write(disk_fd, data_buf, bytes_to_write);
		flbyte_cnt -= bytes_to_write;
		printf("%d bytes written \n", bytes_to_write);
	}
				
	close(disk_fd);
	close(urfl_fd);	
	
	write_metdata(usrflnm, byte_cnt, begloc);
	
	
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


FR_FLGBLK * createblk(int bitloc){

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


int write_flags_todisk(int flg_datasz){
	
	int data_written = 0;
	int fd = open("diskname, O_WRONLY");
	
	lseek(fd, 2, SEEK_SET);
	data_written = write(fd, flags, flg_datasz);
	close(fd);
	if(data_written != flg_datasz)
		return -1;
	return 0;
	
}	



int main(int argc, char *argv[]){
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
		exit(EXIT_FAILURE);
	}
	
	diskname = argv[1];
	unsigned char ch;
	int fd = open(diskname, O_RDONLY);
	int rdcnt;
	int readsize;
	long int usrflsz;

	if(fd){
		read(fd, &ch, sizeof(char));
		dsksz = (long int)pow(2, ch);

		read(fd, &ch, sizeof(char));
		blksz = (long int)pow(2, ch);
	
		blkcnt = dsksz/blksz;
		flgblkcnt = (blkcnt/8) / blksz; 
	
		readsize = flgblkcnt * blksz;	
		readsize -= (flgblkcnt/(ULBCNT/8));
		chunks = readsize/(ULBCNT/8);
	
		flags = malloc(readsize);
		rdcnt = read(fd, flags, readsize);
		
		lseek(fd, 3, SEEK_END);
		read(fd, &ttlmetadata_sz, sizeof(ttlmetadata_sz));
		
		close(fd);

		while(1){
		
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
			
			fd = open("b.txt", O_RDONLY);
			if(fd){
				usrflsz = lseek(fd, 0, SEEK_END);
				close(fd);
				if(write_to_file("b.txt", ((FFLST.head->loc+flgblkcnt)*blksz)+1, usrflsz) == -1){
					perror("Failed to write file to disk");
					exit(EXIT_FAILURE);
				}
				else{   // update flag values inside the disk
					if(write_flags_todisk(readisize) == -1){
						perror("Failed to write flags\n");
						exit(EXIT_FAILURE);
					}
				}
			
			}
			
			display_flags();
			
			
		}
		
		

		

	}
	else{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

		
	
	return 0;
}
