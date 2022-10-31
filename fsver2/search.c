#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#include "vdfile_metadata.h"
#include "vdrun_disk.h"
#include "vddriver.h"
#include "vddiskinfo.h"

int search( FILE_ACTION_VARS *FAV, int get_empty_block){
  
    FL_METADATA *flmtdptr;
    char *buffer = malloc(sizeof(char) * FAV->DSKINF.blksz);
    unsigned int ttl_mtdblks_in_dskblk = FAV->DSKINF.blksz/sizeof(FL_METADATA);
    int found = 0;
    int i, j;
    
    i = 1;
    while(i<=FAV->DSKINF.ttlmtdta_blks){

        memset(buffer, 0, FAV->DSKINF.blksz);
        vdread(FAV->disk_fd, buffer, i, FAV->DSKINF.blksz); //read the block containing the metadatablocks
        flmtdptr = (FL_METADATA *)buffer;

        for(int j = 0; j<ttl_mtdblks_in_dskblk; j++){ // loop untill the there are no more blocks in buffer or either there are no more metadata blocks in file
            if(get_empty_block){
                if(flmtdptr->isavailable){
                    found = 1;
                    break;
                }
            }
            else if(!flmtdptr->isavailable && !(strcmp(FAV->usrflnm, flmtd->flnm))){   
                found = 1;
                break;
            }

            if(found) break;
            
            flmtdptr++;
        }

        i++;
    }

    free(buffer);

    if(found){
        FAV->dskblk_ofmtd = i;
        FAV->loc_ofmtd_in_blk = j;
        return 0;
    }

    return -1;
}
 