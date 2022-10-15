#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vdheader.h>
#include "vdsyslib.h"

int deletefile(char *filename){
  
	unsigned int disk_fd = open(DSKINF.diskname, O_RDWR);
   
    int i = 1;
    int curblock = DSKINF.dsksz/DSKINF.blksz;
  
    while(i<=ttlmetadatablks){

        memset(buffer, '\0', DSKINF.blksz);
        vdread(fd, buffer, curblock-i, DSKINF.blksz);
        int mtdblks = (pow(2,blksz))/sizeof(FL_METADATA);
        char *buffer2 = malloc(sizeof(char) * DSKINF.blksz);
        int *buffer3 = malloc(sizeof(int) * DSKINF.blksz);
        int *blk = NULL;
        int flag = 0;
        int u = 0;
        flmtd = (FL_METADATA *)buffer;

        for(int j = 0; j<mtdblks && i<=ttlmetadatablks; j++){

            if(flmtd->isavailable){

                if(!strcmp(filename, flmtd->flnm)){
             
                    int tempk = flmtd->strtloc;
                    
                    for(int l = 0; l<blocksdata_blocks; l++){
                        
                        memset(buffer2, '\0', DSKINF.blksz);
                        vdread(fd, buffer2, tempk, DSKINF.blksz);
                        u = 0;
                        blk = (int *)&buffer2[0];
                        

                        memset(buffer3, '\0', DSKINF.blksz);
                        while(*blk > 0 && u!= 255){

                            buffer3[u] = *blk;
                            u++;
                            blk++;
                           
                        }
                        setbits(buffer3, u-1, 1);
                        tempk = *blk;
                    }

                    flmtd->isavailable = 0;
                    flag = 1;
                    break;
                }

            }
            flmtd++;
            i++;
        }

        if(flag) break;

    }

  close(disk_fd);

  return 0;
}
