#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vdheader.h>
#include "vdsyslib.h"

int deletefile(char *filename){
  
	unsigned int disk_fd = open(DSKINF.diskname, O_RDWR);
   
    char *buf2 = malloc(sizeof(char) * DSKINF.blksz);
    int i = 1;
    int curblock = DSKINF.dsksz/DSKINF.blksz;
  
    while(i<=ttlmetadatablks){

        vdread(fd, buf2, curblock-i, DSKINF.blksz);
        int mtdblks = (pow(2,blksz))/sizeof(FL_METADATA);
        int *buffer2 = malloc(sizeof(int) * DSKINF.blksz);
        int *blk = NULL;
        int flag = 0;
        int u = 0;
        flmtd = (FL_METADATA *)buf2;

        for(int j = 0; j<mtdblks && i<=ttlmetadatablks; j++){

            if(flmtd->isavailable){

                if(!strcmp(filename, flmtd->filename)){
             
                    int tempk = flmtd->strtloc;
                    
                    for(int l = 0; l<blocksdata_blocks; l++){
                        
                        vdread(fd, buffer, tempk, DSKINF.blksz);
                        u = 0;
                        blk = (int *)&buffer[0];
                        memset(buffer, '\0', 1024);

                        while(*blk > 0 && u!= 255){

                            buffer2[u] = *blk;
                            u++;
                            blk++;
                           
                        }
                        setbits(buffer2, u-1, 1);
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
