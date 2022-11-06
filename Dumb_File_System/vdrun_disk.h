#ifndef _VD_RUN_DISK_H
#define _VD_RUN_DISK_H

#include "vdfile_action_vars.h"

#ifndef _INSERT_FILE
#define _INSERT_FILE
int insert_file( FILE_ACTION_VARS *);
#endif

#ifndef _SEARCH_FILE
#define _SEARCH_FILE
int search( FILE_ACTION_VARS *, int);
#endif

#ifndef _FETCH_FILE
#define _FETCH_FILE
int fetch(FILE_ACTION_VARS *);
#endif

#ifndef _DELETE_FILE
#define _DELETE_FILE
int delete(FILE_ACTION_VARS *);
#endif

#ifndef _GET_TREE_INFO
#define _GET_TREE_INFO
int get_tree_info(FILE_ACTION_VARS *);
#endif

#endif


