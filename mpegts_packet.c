#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpegts_packet.h"
#include "mpegts_header.h"
#include "utils.h"

#define PACKET_SIZE 188

void init_mpegts_reader_data(char* filein, mpegts_reader_data* rd, size_t buffer_max, size_t read_more){
    rd->read_more = read_more;
    rd->f = fopen(filein,"r");
    if(NULL == rd->f){
        fprintf(stderr, "ERROR: could not open file %s\n",filein);
        exit(1);
    }
    rd->eof = 0;
    rd->started_reading = 0;
    if(buffer_max < 5000){
        printf("your buffer max is really small, using 5000 instead\n");
        buffer_max = 5000;
    }
    if(buffer_max > 1000000){
        printf("your buffer max is really big, using 1000000 instead\n");
        buffer_max = 1000000;
    }
    rd->buffer_max = buffer_max;
    rd->buffer = (uint8_t*) malloc(buffer_max);
    if(NULL == rd->buffer){
        fprintf(stderr, "ERROR: was not able to allocate memory for buffer\n");
        exit(1);
    }
    rd->current_offset = 0;//buffer_max;
    rd->buffer_size = 0;
    return;
}

void read_more_ts_data(mpegts_reader_data* rd){
    if( (rd->buffer_size - rd->current_offset) > rd->read_more)
        return;
    if(rd->eof)
        return;

    // if current offset is not zero, then we need to copy what's lef of the buffer at the
    // beginning
    size_t bytes_saved = 0;
    if(rd->current_offset > 0){
        bytes_saved = rd->buffer_size - rd->current_offset - 188;
        void *ret = memcpy(rd->buffer, rd->buffer + rd->current_offset + 188, bytes_saved);
        if(NULL == ret){
            fprintf(stderr, "error copying buffer data\n");
            exit(1);
        }
    }

    size_t bytes_read = 0;
    if(! rd->eof){
        bytes_read = fread(rd->buffer + bytes_saved, 
                           sizeof(char), 
                           rd->buffer_max - bytes_saved, 
                           rd->f);
        if(bytes_read == 0)
            rd->eof = 1;
        //printf("reading %zu more bytes\n", bytes_read);
    }
    rd->buffer_size = bytes_saved + bytes_read;
    rd->current_offset = 0;
    return;
}

int has_next_ts_packet(mpegts_reader_data* rd){
    read_more_ts_data(rd);
    //printf("max: %zu, -- size: %zu, -- offset: %zu\n", rd->buffer_max, rd->buffer_size, rd->current_offset);

    // case 1: still data to read on buffer
    if((rd->current_offset + PACKET_SIZE) < rd->buffer_size)
        return 1;

    return 0;
}

void get_next_ts_packet(mpegts_reader_data* rd, ts_packet* ts){
    size_t NUM_OF_ADAPTATION_BYTES = 4;
    if(!has_next_ts_packet(rd)){
        fprintf(stderr, "error, you are requesting a new ts packet but there aren't any");
        exit(1);
    }
    if(rd->started_reading == 0){
        rd->current_offset += 0;
        rd->started_reading = 1;
    }
    else {
        rd->current_offset += 188;
    }

    ts->header_raw = uint8_ptr_to_uint32_big_endian(rd->buffer + rd->current_offset);        
    ts->raw_data = rd->buffer + rd->current_offset;
    read_ts_packet_header(&ts->h, ts->raw_data);
    ts->payload = ts->raw_data + 4 + ts->h.adaptation.adaptation_length;

    // uint8_t next_header = rd->buffer[rd->current_offset];
    // if(next_header != 0x47){
    //     int i;
    //     for(i = -10; i < 10; ++i)
    //         printf("%d: %x\n",i,rd->buffer[rd->current_offset + i]);
    //     printf("oh oh\n");
    // }
    return;
}

void close_mpegts_reader_data(mpegts_reader_data* rd){
    if(rd->f)
        fclose(rd->f);
    if(rd->buffer)
        free(rd->buffer);
    return;
}

// void read_next_ts_packet(char *buffer, char **next_buffer, char **ts_packet, 
//                          unsigned int *ts_packet_size, unsigned int* header,
//                          mpegts_header* formatted_header, mpegts_adaptation* a){

//     unsigned int h = charptr_to_unsigned_int_big_endian(buffer);

//     if(header){
//         *header = h;
//     }
//     if(!formatted_header){
//         fprintf(stderr,"error in read_next_ts_packet, formatted_header is NULL\n");
//         exit(1);
//     }
//     read_ts_packet_header(formatted_header, buffer);
//     *ts_packet = buffer + 4; // move 4 bytes (passed the header)

//     switch(formatted_header->adaptation){
//         case ((unsigned int) 0):
//             // 00 – RESERVED for future use [9] 
//             fprintf(stderr,"adaptation field is zero\n");
//             exit(1);
//         case ((unsigned int) 1):
//             // 01 – no adaptation field, payload only,
//             // ---> read_ts_packet_adaptation will just ignore it for now
//         case ((unsigned int) 2):
//         case ((unsigned int) 3):
//             // 10 – adaptation field only, no payload,
//             // 11 – adaptation field followed by payload, 
//             // in both cases we need to read the adaptation field,
//             read_ts_packet_adaptation(a, formatted_header, buffer + 4);
//         default: 
//             break;
//     }
//     return; 
//     // *ts_packet_size = 184 - a->adaptation_length;
//     // *ts_packet = buffer + 4 + a->adaptation_length;
//     // return;
// }

void print_line(uint8_t* data, size_t num_items){
    for(int i = 0; i < num_items; ++i){
        printf("%02X", data[i]);
        if(i != 0 && (i+1) % 2 == 0)
            printf(" ");
    }
    printf("\n");
}
    
void print_ts_packet(ts_packet* ts, size_t num_blank_lines, int print_ahead){
    uint8_t *data = ts->raw_data;
    size_t line_size = 16;
    size_t packet_size = 188;
    // print 2 lines of 4 ints each    
    for(int i = 0; i < 2*line_size; i += line_size){
        // each line is 16 bytes
        print_line(data + i, line_size);
    }
    if(num_blank_lines > 0)
        printf(" ... skipping %lu lines\n", num_blank_lines);
    for(int i =  (2 + num_blank_lines)*line_size; i < 12*line_size; i += line_size){
        size_t num_left = packet_size - i;
        size_t num_print = num_left > line_size ? line_size : num_left;
        print_line(data + i, num_print);
    }

    if(print_ahead){
        // look ahead
        printf("bytes ahead:  ");
        print_line(data + packet_size, 32);
        printf("\n");
    }
    // printed 16 bytes
    return;
}