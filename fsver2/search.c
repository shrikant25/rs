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
    
    i = FAV->DSKINF.mtdta_blk_ofst;
    while(i<=FAV->DSKINF.dsk_blk_for_mtdata){

        memset(buffer, '\0', FAV->DSKINF.blksz);
        int amount = vdread(FAV->disk_fd, buffer, i, FAV->DSKINF.blksz); //read the block containing the metadatablocks
       // printf("amoutn %d\n", amount);
        flmtdptr = (FL_METADATA *)buffer;

        for(j = 0; j<ttl_mtdblks_in_dskblk; j++){ // loop untill the there are no more blocks in buffer or either there are no more metadata blocks in file
        //    printf("name %s\n", flmtdptr->flnm);
            if(get_empty_block){
        //        printf("----   %d\n",flmtdptr->isavailable);
          //      printf("----  %d\n",flmtdptr->flsz);
                if(flmtdptr->isavailable == 1){
                    found = 1;
                    break;
                }
            }
            else if(!flmtdptr->isavailable && !(strncmp(FAV->usrflnm, flmtdptr->flnm, strlen(FAV->usrflnm)))){  
                FAV->filebegloc = flmtdptr->strtloc; 
                found = 1;
                break;
            }

          
            
            flmtdptr++;
        }
  if(found) break;
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
 