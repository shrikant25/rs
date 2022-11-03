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
	//store filename in structure variable 
		printf("filenmae %ld %s \n",strlen(flmtd.flnm),  flmtd.flnm);
		printf(" %d\n", flmtd.flsz);
		printf("%d\n", FAV->dskblk_ofmtd);
		printf("%d\n", FAV->loc_ofmtd_in_blk);
	//store bytes of structure variable in buffer at given location
	
	write_to_buffer(buffer, (char *)&flmtd, sizeof(FL_METADATA), sizeof(FL_METADATA)*FAV->loc_ofmtd_in_blk);
	FL_METADATA *flmtdptr = (FL_METADATA *)buffer;
		printf("filenmae %ld %s \n",strlen(flmtdptr->flnm),  flmtdptr->flnm);
		printf(" %d\n", flmtdptr->flsz);
		printf("%d\n", FAV->dskblk_ofmtd);
		printf("%d\n", FAV->loc_ofmtd_in_blk);
	
	// write metadata to disk
	vdwrite(FAV->disk_fd, buffer, FAV->dskblk_ofmtd, FAV->DSKINF.blksz);
	
	free(buffer);
	return 0;
}