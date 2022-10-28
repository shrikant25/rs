#include <stdio.h>
void write_to_buffer(char *buffer, char *val, unsigned int num_of_bytes, unsigned int index){

    unsigned int i = index;
    unsigned int j = 0;

    while(j<num_of_bytes){
        buffer[i++] = val[j];
        j++;
    }
}