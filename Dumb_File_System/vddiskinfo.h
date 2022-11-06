#ifndef _VD_DSK_INFO_H
#define _VD_DSK_INFO_H

#ifndef _DISKINFO
#define _DISKINFO
typedef struct DISKINFO{
	char * diskname;
	unsigned int ttlmtdta_blks;
	unsigned int mtdta_blk_ofst;
	unsigned long int dsksz;
	unsigned long int blksz;
	unsigned long int blkcnt;
	unsigned int flgblkcnt;
    unsigned int flags_arrsz;
    unsigned int dsk_blk_for_mtdata;
}DISKINFO;
#endif

#endif