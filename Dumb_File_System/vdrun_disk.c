#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vddriver.h"
#include "vdsyslib.h"
#include "vdrun_disk.h"
#include "vdwrite_to_buffer.h"
#include "vdwrite_flags_to_disk.h"
#include "vdconstants.h"
#include "test.h"

int perform_task(char * task, char *path, FILE_ACTION_VARS FAV){
	
	int level_size[5]; 
	FAV.level_size = level_size;
	
	if(!strcmp(task, "task_insert_file")){
								
		FAV.usrfl_fd = open(path, O_RDONLY, 00777);
			if (FAV.usrfl_fd == -1) return -1;
		
		FAV.usrflsz = lseek(FAV.usrfl_fd, 0, SEEK_END);
			lseek(FAV.usrfl_fd, 0, SEEK_SET);

		search(&FAV, 1);
		if(FAV.dskblk_ofmtd != -1 && FAV.loc_ofmtd_in_blk != -1){
			get_tree_info(&FAV);
			insert_file(&FAV);
			close(FAV.usrfl_fd);
			return 0;
		}
		return -1;

	}
	else if(!strcmp(task, "task_delete_file")){

		FAV.usrfl_fd = -1;
		search(&FAV, 0);

		if(FAV.dskblk_ofmtd != -1 && FAV.loc_ofmtd_in_blk != -1){
			get_tree_info(&FAV);
			delete(&FAV);
			return 0;
		}
		return -1;
	}
	else if(!strcmp(task, "task_fetch_file")){

		FAV.usrfl_fd = open(path, O_WRONLY | O_CREAT, 00777);
		if (FAV.usrfl_fd == -1) return -1;
		
		FAV.usrflsz = lseek(FAV.usrfl_fd, 0, SEEK_END);
			lseek(FAV.usrfl_fd, 0, SEEK_SET);

		search(&FAV, 0);
		if(FAV.dskblk_ofmtd != -1 && FAV.loc_ofmtd_in_blk != -1){
			get_tree_info(&FAV);
			fetch(&FAV);
			close(FAV.usrfl_fd);
			return 0;
		}
		return -1;
	}
	else
		return -2;
}


int run_disk(DISKINFO DSKINF, FR_FLGBLK_LST *FFLST, unsigned long int *flags, int fd){

	int task_file_fd = open("task_file", O_RDONLY, 00777);
	if(task_file_fd == -1) return -1;
	int status;
	FILE_ACTION_VARS FAV;
	FAV.DSKINF = DSKINF;
	FAV.FFLST = FFLST;
	FAV.flags = flags;
	FAV.disk_fd = fd;
	TASK_NODE *buffer = malloc(sizeof(TASK_NODE) * 10); 
	
	int task_cnt;
	int data_read = read(task_file_fd, &task_cnt, sizeof(int));
	printf("%d\n", task_cnt);

	for(int j = 0; j<task_cnt/10; j++){
		memset(buffer, 0, sizeof(TASK_NODE) * 10);
		data_read = read(task_file_fd, buffer, sizeof(TASK_NODE) * 10);
		
		for(int i = 0; i<10; i++){
			
			FAV.level_size = NULL;
		
			FAV.tree_depth = -1;
			FAV.dskblk_ofmtd = -1;
			
			FAV.loc_ofmtd_in_blk = -1;
			FAV.usrfl_fd = -1;
			FAV.usrflnm = malloc(sizeof(char) * strlen(buffer[i].filename));
			strcpy(FAV.usrflnm ,buffer[i].filename);
			FAV.usrflsz = -1;
			FAV.filebegloc = -1;
			
			status = perform_task(buffer[i].task, buffer[i].path, FAV);
			free(FAV.usrflnm); 
			
		}
	
	}

	free(buffer);
	return 0;
}

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
	
	int data_read;
	if(argc != 2){
		printf("Invalid arguments\n");
		exit(EXIT_FAILURE);
	}
	
	unsigned int fd = 0; 
	unsigned long int *flags;
	int task = 0;
	DISKINFO DSKINF;
	FR_FLGBLK_LST FFLST;
	FFLST.frblkcnt = 0;
	FFLST.head = NULL;
	
	fd = open(argv[1], O_RDWR, 00777);
	if(fd == -1) return -1;

	data_read = read(fd, &DSKINF, sizeof(DSKINF));
	
	flags = malloc(DSKINF.flags_arrsz * VDQUAD);
	readflags(fd, flags, DSKINF);
	
	build(DSKINF, flags, &FFLST);		
	
	int status = run_disk(DSKINF, &FFLST, flags, fd);
	write_flags_todisk(fd, flags, DSKINF);
	free(flags);
	close(fd);
	
	return 0;
}
