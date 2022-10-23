#ifndef _VDHEADER_H
#define _VDHEADER_H

#ifndef _FL_METADATA
#define _FL_METADATA
typedef struct FL_METADATA{
	char flnm[52];
	unsigned int strtloc;
	unsigned int flsz;
	unsigned int isavailable;
}FL_METADATA;
#endif

#endif