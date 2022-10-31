#ifndef _FILE_ACTION_VARS_H
#define _FILE_ACTION_VARS_H

#include "vdget_empty_blocks.h"

#ifndef _FILE_ACTION_VARS
#define _FILE_ACTION_VARS
typedef struct FILE_ACTION_VARS{
	int disk_fd;
	int usrfl_fd; 
	char *usrflnm;
	unsigned int usrflsz;
	unsigned long int *flags;  
	unsigned int *level_size; 
	unsigned int tree_depth; 
	unsigned int dskblk_ofmtd;
	unsigned int loc_ofmtd_in_blk;
	unsigned int filebegloc;
	FR_FLGBLK_LST *FFLST;
	DISKINFO DSKINF;
}FILE_ACTION_VARS;
#endif


#endif