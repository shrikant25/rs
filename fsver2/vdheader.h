#ifndef _COMMONVAR_H
#define _COMMONVAR_H


#ifndef _BUFLEN
#define BUFLEN 1024
#endif

#ifndef _ULBCNT
#define ULBCNT 64 
#endif

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

#ifndef _FL_METADATA
#define _FL_METADATA
typedef struct FL_METADATA{
	char flnm[ULBCNT];
	unsigned int isavailable;
	unsigned int strtloc;
	unsigned int flsz;
}FL_METADATA;
#endif

#ifndef _DISKINFO
#define _DISKINFO
typedef struct DISKINFO{
	const char * diskname;
	unsigned int ttlmetadata_blks;
	unsigned long int dsksz;
	unsigned long int blksz;
	unsigned int blkcnt;
	unsigned int flgblkcnt;
}DISKINFO;
#endif

#ifndef _DSKINF
#define _DSKINF
extern DISKINFO DSKINF;
#endif

#ifndef _FFLST
#define _FFLST
extern FR_FLGBLK_LST FFLST;
#endif

#ifndef _BUFFER
#define _BUFFER
extern char *buffer;
#endif

#ifndef _FLAGS
#define _FLAGS
extern unsigned long int *flags;
#endif

#ifndef _VDREAD
#define _VDREAD
int vdread(int, char*, int, int);
#endif

#ifndef _VDWRITE
#define _VDWRITE
int vdwrite(int, char*, int , int);
#endif

#ifndef _CREATEFILE
#define _CREATEFILE
int createfile(char *, unsigned long int);
#endif

#ifndef _SETBITS
#define _SETBITS
int setbits(unsigned int, unsigned long int, unsigned int);
#endif


#endif


