#ifndef _GET_EMPTY_BLOCKS_H
#define _GET_EMPTY_BLOCKS_H

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

#ifndef _GETEMPTY_BLOCKS
#define _GETEMPTY_BLOCKS
int getempty_blocks(int, unsigned int *, FR_FLGBLK_LST *);
#endif

#ifndef _BUILD
#define _BUILD
void build(DISKINFO, unsigned long int *, FR_FLGBLK_LST *);
#endif

#endif