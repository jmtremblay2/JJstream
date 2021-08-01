#ifndef MPEGTS_PACKET
#define MPEGTS_PACKET

#define TS_PACK_SIZE 188

#include "mpegts_header.h"

#include <stdint.h>
#include <stdint.h>

typedef struct ts_packet {
    uint32_t header_raw;
    uint8_t *raw_data;
    uint8_t *payload;
    size_t payload_size;
    mpegts_header h;
    mpegts_adaptation a;
} ts_packet;

typedef struct mpegts_reader_data {
    char file_name[200];

    FILE* f;
    int eof;
    int started_reading;
    uint8_t* buffer;
    size_t buffer_max;
    size_t buffer_size;
    size_t read_more;
    size_t current_offset;
} mpegts_reader_data;

void init_mpegts_reader_data(char* filein, mpegts_reader_data* rd, size_t buffer_max, size_t read_more);
void read_more_ts_data(mpegts_reader_data* rd);
int has_next_ts_packet(mpegts_reader_data* rd);
void get_next_ts_packet(mpegts_reader_data* rd, ts_packet* ts);
void close_mpegts_reader_data(mpegts_reader_data* rd);
// void read_next_ts_packet(char *buffer, 
//                          char **next_buffer, 
//                          char **ts_packet, 
//                          unsigned int *ts_packet_size, 
//                          unsigned int* header,        
//                          mpegts_header* formatted_header,
//                          mpegts_adaptation* a);
void print_line(uint8_t* data, size_t num_items);
void print_ts_packet(ts_packet* ts, size_t num_blank_lines, int print_ahead);

#endif