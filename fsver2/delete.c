#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vdsyslib.h"
#include "vdheader.h"

int deletefile(char *filename){
  
	int disk_fd = open(DSKINF.diskname, O_RDWR);
   
    if(disk_fd){
        
        int i = 1;
        int curblock = DSKINF.dsksz/DSKINF.blksz;
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

        // variable to store current int block
        unsigned int tempcurrent_block = 0;

        unsigned int *blk = NULL;
        int u,l,j, i = 0;

        // pointer to point to individual blocks in buffer
        FL_METADATA *flmtd;   

        // loop untill all metadatablocks are read
        while(i<=DSKINF.ttlmetadata_blks){

            memset(buffer, '\0', DSKINF.blksz);
            // read the last block of disk, because metadata is stored from last block
            vdread(disk_fd, buffer, curblock-i, DSKINF.blksz);

            // point to  buffer with a pointer of type FL_METADAT
            flmtd = (FL_METADATA *)buffer;

            // loop untill there are no more metadatablocks to be read or entire buffer is read
            for(j = 0; j<mtdblks && i<=DSKINF.ttlmetadata_blks; j++){
                
                // check if block is set to available 
                // block is only set to available when it contains data of a file that is present in disk
                // and if filename in block matches with the filename given as input
                if(flmtd->isavailable && (!strcmp(filename, flmtd->flnm))){ 

                    //store value of first block that contains integers
                    tempcurrent_block = flmtd->strtloc;
                    
                    // based on file size calculate how many blocks will be required to store file data
                    // based on those blocks, calculate how many blocks will be required to store integer values of data blocks
                    datablocks = ceil((float)flmtd->flsz/(float)DSKINF.blksz);
                    blocksof_ints = ceil((float)datablocks/(float)blockint_capacity);
                    totalblocks = datablocks + blocksof_ints;
                    
                    //read all the blocks containing the integers

                    for(l = 0; l<blocksof_ints; l++){
                        
                        memset(buffer2, '\0', DSKINF.blksz);
                        // read the block
                        vdread(disk_fd, buffer2, tempcurrent_block, DSKINF.blksz);
                        u = 0;

                        //point to first integer value in buffer
                        blk = (int *)&buffer2[0];

                        // iterate untill all values in buffer are read or no more values are left(i.e a zero has occured)
                        while(*blk > 0 && u!= 255){

                            setbits(blk, 1, 1); // set the bit for particular data block as 1 (i.e it is now availabel to write)
                            u++;
                            blk++;
                            
                        }

                        setbits(&tempcurrent_block, 1, 1); // set the bit for particular integer block as 1 (i.e it is now availabel to write)
                        tempcurrent_block = *blk; // point to next integer block using the last value in buffer
                    }

                    flmtd->isavailable = 0; // set the block is unavailable
                    close(disk_fd);
                    return 0;
                }
                
                flmtd++; // represents the metatdata count in buffer, the buffer can contain available as well as unavailabel blocks

                if(flmtd->isavailable) // if only a availabel block is read only then increment the count 
                    i++;
            }

        }
        close(disk_fd);
    }
    return -1;
}
