#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#include "vdheader.h"

int deletefile(char *filename){
  
	int disk_fd = open(DSKINF.diskname, O_RDWR);
   
    int i = 1;
    int curblock = DSKINF.dsksz/DSKINF.blksz;
  
    // loop untill all metadatablocks are read
    while(i<=DSKINF.ttlmetadata_blks){

        // total metadatablocks that can be stored in a disk block
        int mtdblks = DSKINF.blksz/sizeof(FL_METADATA);

        // varialbe to store the number of blocks required to store the file data
        // these blocks will contain integer values of blocks where data is stored
        // the bits representing these blocks as well as of those blocks that are being
        // represented by intgeres present in these blocks are currently set to 0 means occupied
        // these all bits need to be set to 1(means available)   
        unsigned int blocksof_ints = 0; // blocks to store intgers
        unsigned int datablocks = 0; // blocks to store file data
        unsigned int totalblocks = 0; // combination of above two
        unsigned int blockint_capacity = DSKINF.blksz/sizeof(int); // number of integers that can be stored in a block;
        blockint_capacity -= 1; // because last value of all integers blocks(except for last one) is just the value of next block(we dont need that)


        //buffer to store integers 
        char *buffer2 = malloc(sizeof(char) * DSKINF.blksz);

        // varilabel
        unsigned int tempcurrent_block = 0;
        int *blk = NULL;
        int flag = 0;
        int u = 0;

        // pointer to point to individual blocks in buffer
        FL_METADATA *flmtd;   


        memset(buffer, '\0', DSKINF.blksz);
        // read the last block of disk, because metadata is stored from last block
        vdread(disk_fd, buffer, curblock-i, DSKINF.blksz);

        // point to  buffer with a pointer of type FL_METADAT
        flmtd = (FL_METADATA *)buffer;

        // loop untill there are no more metadatablocks to be read or entire buffer is read
        for(int j = 0; j<mtdblks && i<=DSKINF.ttlmetadata_blks; j++){
            
            // check if block is set to available 
            // block is only set to available when it contains data of a file that is present in disk
            // and if filename in block matches with the filename given as input
            if(flmtd->isavailable && (!strcmp(filename, flmtd->flnm))){ 

                //
                unsigned int tempk = flmtd->strtloc;
                // based on file size calculate how many blocks will be required to store file data
                // based on those blocks, calculate how many blocks will be required to store integer values of data blocks
                unsigned int datablocks = ceil((float)flmtd->flsz/(float)DSKINF.blksz);
                unsigned int blocksof_ints = ceil((float)datablocks/(float)blockint_capacity);
                unsigned int totalblocks = datablocks + blocksof_ints;
                
                //read all the blocks containing the integers

                for(int l = 0; l<blocksof_ints; l++){
                    
                    memset(buffer2, '\0', DSKINF.blksz);
                    vdread(disk_fd, buffer2, tempk, DSKINF.blksz);
                    u = 0;
                    blk = (int *)&buffer2[0];

                    memset(buffer3, '\0', DSKINF.blksz);
                    while(*blk > 0 && u!= 255){

                    setbits(buffer3, u-1, 1);
                        u++;
                        blk++;
                        
                    }
                    setbits(&tempk, 1, 1);
                    tempk = *blk;
                }

                flmtd->isavailable = 0;
                flag = 1;
                break;
            }

            flmtd++;
            i++;
        }

        if(flag) break;

    }

  close(disk_fd);

  return 0;
}
