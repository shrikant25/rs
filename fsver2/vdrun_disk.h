#ifndef _VDHEADER_H
#define _VDHEADER_H

#include "vddiskinfo.h"

#ifndef _FR_FLGBLK
#define _FR_FLGBLK
typedef struct FR_FLGBLK{
	unsigned int loc;
	unsigned int cnt;
	struct FR_FLGBLK *next;
}FR_FLGBLK;
#endif

#ifndef _FR_FLGBLK_LST
#define _FR_FLGBLK_LST
typedef struct FR_FLGBLK_LST{
	FR_FLGBLK *head;
	unsigned int frblkcnt;
}FR_FLGBLK_LST;
#endif

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
	FR_FLGBLK_LST *FFLST;
	DISKINFO DSKINF;
}FILE_ACTION_VARS;
#endif


#ifndef _GETEMPTY_BLOCKS
#define _GETEMPTY_BLOCKS
int getempty_blocks(int, unsigned int *, FR_FLGBLK_LST *);
#endif

#ifndef _BUILD
#define _BUILD
void build(DISKINFO, unsigned long int *, FR_FLGBLK_LST *);
#endif

#ifndef _INSERT_FILE
#define _INSERT_FILE
unsigned int insert( FILE_ACTION_VARS *FAV, unsigned int parent_block){
#endif

#endif


