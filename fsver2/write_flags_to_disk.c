#include "vdconstants.h"
#include "vdsyslib.h"
#include "vddiskinfo.h"

int write_flags_todisk(unsigned long int *flags, DISKINFO DSKINF){
	
	char *chptr;
	char *buffer = calloc(sizeof(DSKINF.blksz))
	unsigned int fd = open(DSKINF.diskname, O_WRONLY);
	int i, fd,
	
	if(fd){
		chptr = (char *)flags;
    	for(i = 1; i<=DSKINF.flgblkcnt; i++){
			memset(buffer, 0, DSKINF.blksz);
			write_to_buffer(buffer, chptr, DSKINF.blksz, 0);
			vdwrite(fd, buffer, i, blksz);
			chptr = chptr + blksz;
	    }
		close(fd);
		return 0;
	}
	return -1;

}	