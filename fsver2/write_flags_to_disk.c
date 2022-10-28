#include "vdconstants.h"
#include "vdsyslib.h"
#include "vddiskinfo.h"
#include "vddriver.h"
#include "vdwrite_to_buffer.h"
#include <stdlib.h>
#include <string.h>

int write_flags_todisk(unsigned long int *flags, DISKINFO DSKINF){
	
	char *chptr;
	char *buffer;
	int fd = open(DSKINF.diskname, O_WRONLY);
	int i = 0;
	
	if(fd){
		buffer = malloc(sizeof(DSKINF.blksz));
		chptr = (char *)flags;
    	for(i = 1; i<=DSKINF.flgblkcnt; i++){
			memset(buffer, 0, DSKINF.blksz);
			write_to_buffer(buffer, chptr, DSKINF.blksz, 0);
			vdwrite(fd, buffer, i, DSKINF.blksz);
			chptr = chptr + DSKINF.blksz;
	    }
		
		close(fd);
		free(buffer);
		return 0;
	}
	
	return -1;

}	