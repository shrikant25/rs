#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>


const char * sz_infofl = "szlog_fl"; 							// size info
const char * sz_avl_infofl = "sz_avl_infofl"; 						// size availability info
const char * lck_stsfl = "lck_stsfl"; 							// file to check if some other processes have locked all the resources(isbusy_flag)


void throw_error(char * errormsg);
int change_lock_status(unsigned char);
int write_sharedsize_file(unsigned long int);
int empty_file(const char *);
int check_mem_avbl();
int set_lock();


void cleanup(){
	
	change_lock_status((unsigned char)0); 						// as file is sucessfully written to the disk, change the status in lock file to unoccupied(0)		
	empty_file(sz_infofl);								// empty all the contents written in shared size file
	empty_file(sz_avl_infofl);							// empty all the contents written in shared status file						

}


// throws a error and exits the program
void throw_error(char * errormsg){
	perror(errormsg);
	exit(EXIT_FAILURE);
}


// empties all the contents of given file
int empty_file(const char * filename){

	int fd = open(filename, O_WRONLY); 						// empties the given file by truncating(starting from end) till the first position
	if(fd){
		ftruncate(fd, 0);
		close(fd);
	}
	else return -1;
	
	return 0;

}


// check if there is any memory location on disk where user's file can be stored
int check_mem_avbl(){

	unsigned int mem_loc = -1;
	int fd = open(sz_avl_infofl, O_RDONLY);
	if(fd){								
		while(!read(fd, &mem_loc, sizeof(unsigned int)));			// continuously read untill some data is found
		return mem_loc;								// return whatever is read
	}
	else return -1;

}


// write the size of user's file to shared size file
// disks memory management program will read this input and will try to find if this much memory can be allocated
// disks memory management program will proved its response in sz_avl_infofl
int write_sharedsize_file(unsigned long int filesize){

	int fd = open(sz_infofl, O_WRONLY); 					
	if(fd){	
		write(fd, &filesize, sizeof(unsigned long int));			// write user's file size to shared file
		close(fd);
	}
	else return -1;

	return 0;
}


// changes status in lock staus file
int change_lock_status(unsigned char status){
	
	int fd = open(lck_stsfl, O_WRONLY); 						// open lock status file  
	
	if(fd){ 
		write(fd, &status, sizeof(unsigned char));				// write status to status file
		close(fd);
	} 
	else return -1;	
	
	return 0;
}


// checks the lock file and when there is no lock, try to set a lock
int set_lock(){

	unsigned char isbusy_flag  = 1;

	int fd = open(lck_stsfl, O_RDONLY); 						// open logs file to check if somebody else is using shared files 
	if(fd){

		while(isbusy_flag){   							// read untill somebody else is using
			read(fd, &isbusy_flag, sizeof(unsigned char));
			lseek(fd, SEEK_SET, 0); 	
		}
		isbusy_flag = 1; 
		close(fd);
		
		return change_lock_status(isbusy_flag);    				// put a lock
	}
	else return -1;
	
}


// 2 args required outfile and userfilename
int main(int argc, char *argv[]){ 

	if(argc!=2)
		throw_error("invalid arguments\n");
		
	unsigned int mem_loc = -1; 							// to store the starting point of available memory location
	unsigned long int filesize = 0;
	int write_status = -1;

	int usrfl_fd = open(argv[1], O_RDONLY); 					// open user file
	if(!usrfl_fd)
		throw_error("failed to open user file\n");
			
	else{
		filesize = lseek(usrfl_fd, 0, SEEK_END); 				// go to end position to get size of user file
		
		if(filesize == -1){
			close(usrfl_fd);
			throw_error("error while accessing the filesize\n");
		}
		printf("filesize %ld\n", filesize);
		if(set_lock() == -1){
			cleanup();  							//  abort all operations
			close(usrfl_fd);
			throw_error("unable to open log file\n");
		}
		
	
		if(write_sharedsize_file(filesize) == -1){
			cleanup();  							// if no memory is not available then abort all operations
			close(usrfl_fd);
			throw_error("failed to convery filesize to disk's memory manager\n");	
			
		}
	
	
	
		mem_loc = check_mem_avbl(); 	
		if(mem_loc == -1){
			cleanup();  							// if no memory is not available then abort all operations
			close(usrfl_fd);
			throw_error("enough memory not available\n");
		}
		else{  	 
			printf("to write %ld at %d loc", filesize, mem_loc);								// if memory is available then write the contents of userfile to disk
			//write_status = write_to_disk(usrfl_fd, filesize, mem_loc)
			close(usrfl_fd);
			cleanup();
			write_status = 0;
			if(write_status == -1)
				throw_error("failed to write file\n");
						 
		}
	//cleanup();  
	close(usrfl_fd);
	change_lock_status((unsigned char)0); 				
	}
	
	return 0;
}
