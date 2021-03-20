#include <stdio.h>

#include "mpegts_packet.h"

void print_ts_packet(unsigned char* data, unsigned int bytes_per_line){
    int i = 0, j = 0;

    for(i = 0; i < TS_PACK_SIZE; i+=2){
        if(!(i%10))
            printf("\n");
        printf("%02X%02X ",data[i], data[i+1]);
    }
    printf("\n");

}
