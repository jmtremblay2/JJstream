#ifndef MPEGTS_HEADER
#define MPEGTS_HEADER

typedef struct mpegts_header {
    unsigned int sync;
    unsigned int tei;
    unsigned int pusi;
    unsigned int transport_priority;
    unsigned int pid;
    unsigned int tsc;
    unsigned int adaptation;
    unsigned int continuity_counter;
} mpegts_header;

unsigned int get_header_from_ts(char* ts_packet);
void read_ts_packet_header(mpegts_header* h, char* ts_packet);
void print_ts_packet_header(mpegts_header* h);
void print_ts_packet_pid(mpegts_header* h);

#endif