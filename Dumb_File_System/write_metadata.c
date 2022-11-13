#include <stdlib.h>
#include "vdsyslib.h"
#include <stdio.h>
#include <string.h>
#include "vddriver.h"
#include "vdwrite_to_buffer.h"
#include "vdrun_disk.h"
#include "vdfile_metadata.h"

int write_metadata( FILE_ACTION_VARS *FAV, FL_METADATA flmtd){


	char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
	char *chptr = 0;

	memset(buffer, '\0', FAV->DSKINF.blksz);
	vdread(FAV->disk_fd, buffer, FAV->dskblk_ofmtd, FAV->DSKINF.blksz);
	
	write_to_buffer(buffer, (char *)&flmtd, sizeof(FL_METADATA), sizeof(FL_METADATA)*FAV->loc_ofmtd_in_blk);
	FL_METADATA *flmtdptr = (FL_METADATA *)buffer;
	vdwrite(FAV->disk_fd, buffer, FAV->dskblk_ofmtd, FAV->DSKINF.blksz);
	
	free(buffer);
	return 0;
}