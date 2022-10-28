#include "vdconstants.h"
#include "vdsyslib.h"
#include "vddiskinfo.h"
#include "vddriver.h"
#include "vdwrite_to_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int write_flags_todisk(int fd ,unsigned long int *flags, DISKINFO DSKINF){
	
	char *chptr;
	char *buffer;
	int i = 0;
	
	buffer = malloc(sizeof(DSKINF.blksz));
	chptr = (char *)flags;
	for(i = 1; i<=DSKINF.flgblkcnt; i++){
		memset(buffer, 0, DSKINF.blksz);
		write_to_buffer(buffer, chptr, DSKINF.blksz, 0);
		vdwrite(fd, buffer, i, DSKINF.blksz);
		chptr = chptr + DSKINF.blksz;
	}
	
	free(buffer);
	return 0;

}	