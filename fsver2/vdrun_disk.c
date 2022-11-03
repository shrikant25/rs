#include "vdconstants.h"
#include <stdlib.h>
#include <stdio.h>
#include "vddriver.h"
#include "vdsyslib.h"
#include "vdrun_disk.h"
#include "vdwrite_to_buffer.h"
#include "vdwrite_flags_to_disk.h"
#include <string.h>
#include "test.h"

int perform_task(char * task, FILE_ACTION_VARS FAV){
	strcpy(task, "task_fetch_file");
	FAV.usrfl_fd = open(FAV.usrflnm, O_RDONLY | O_CREAT  , 00777);
		if (FAV.usrfl_fd == -1) return -1;

	int level_size[5]; 
	FAV.level_size = level_size;
	
	FAV.usrflsz = lseek(FAV.usrfl_fd, 0, SEEK_END);
	lseek(FAV.usrfl_fd, 0, SEEK_SET);

	//printf("%ls\n", FAV.level_size);
	//printf("%d\n", FAV.tree_depth);
	if(!strcmp(task, "task_insert_file")){
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
							
								search(&FAV, 0);
								if(FAV.dskblk_ofmtd != -1 && FAV.loc_ofmtd_in_blk != -1){
									get_tree_info(&FAV);
									delete(&FAV);
									close(FAV.usrfl_fd);
									return 0;
								}
								return -1;
	}
	else if(!strcmp(task, "task_fetch_file")){
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
	read(task_file_fd, &task_cnt, sizeof(int));
	printf("%d\n", task_cnt);

	for(int j = 0; j<task_cnt; j++){
		printf("task cnt %d\n", task_cnt);
		memset(buffer, 0, sizeof(TASK_NODE) * 10);
		read(task_file_fd, buffer, sizeof(TASK_NODE) * 10);
		for(int i = 0; i<1; i++){
			
			printf("name %s task %s\n", buffer[i].filename, buffer[i].task);
			FAV.level_size = NULL;
		
			FAV.tree_depth = -1;
			FAV.dskblk_ofmtd = -1;
			
			FAV.loc_ofmtd_in_blk = -1;
			FAV.usrfl_fd = -1;
			FAV.usrflnm = malloc(sizeof(char) * strlen(buffer[i].filename));
			strcpy(FAV.usrflnm ,buffer[i].filename);
			FAV.usrflsz = -1;
			FAV.filebegloc = -1;
			
			status = perform_task(buffer[i].task, FAV);
			free(FAV.usrflnm); 
			
			printf("Task name %s, filename %s, status %d\n", buffer[i].task, buffer[i].filename, status);
			
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
	
	if(argc != 2){
		write(1, "Invalid arguments\n", 18);
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

	read(fd, &DSKINF, sizeof(DSKINF));
	printf("blksz %ld\n", DSKINF.blksz);
	printf("blcknt %ld\n", DSKINF.blkcnt);
	printf("flags arrasz %d\n", DSKINF.flags_arrsz);
	printf("flagsblkcnt %d\n", DSKINF.flgblkcnt);
	printf("ttlmtdta_blks %d\n", DSKINF.ttlmtdta_blks);
	printf(" DSKINF.dsk_blk_for_mtdata %d\n",  DSKINF.dsk_blk_for_mtdata);
	printf(" DSKINF.mtdta_blk_ofst %d\n",  DSKINF.mtdta_blk_ofst);
	printf("flag blocks count : %d\n", DSKINF.flgblkcnt);
	printf("total metadata blocks  %u\n", DSKINF.ttlmtdta_blks);
	printf("dsk blocks required for metadata blocks %d\n", DSKINF.ttlmtdta_blks);
	
	flags = malloc(DSKINF.flags_arrsz * VDQUAD);
	readflags(fd, flags, DSKINF);
	
	build(DSKINF, flags, &FFLST);		
	
	printf("total blcks %d\n", FFLST.frblkcnt);
	printf("largest available block is at %d\n", FFLST.head->loc);
	printf("largest available block is at %ld\n", ((FFLST.head->loc)*DSKINF.blksz)+1);
	printf("total empty  blocks %d\n", FFLST.head->cnt);
	printf("total empty  bytes %ld\n", (FFLST.head->cnt)*DSKINF.blksz);

	int status = run_disk(DSKINF, &FFLST, flags, fd);
	write_flags_todisk(fd, flags, DSKINF);
	printf("status : %d\n", status);
	free(flags);
	close(fd);
	
	return 0;
}
