#ifndef _FL_MTD_HEADER_H
#define _FL_MTD_HEADER_H

#ifndef _FL_METADATA
#define _FL_METADATA
typedef struct FL_METADATA{
	unsigned int strtloc;
	char flnm[52];
	unsigned int flsz;
	unsigned int isavailable;
}FL_METADATA;
#endif

#include "vddiskinfo.h"

#ifndef _VD_WRITE_METADATA
#define _VD_WRITE_METADATA
int write_metadata( FILE_ACTION_VARS *FAV,, FL_METADATA flmtd);
#endif

#endif