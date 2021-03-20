#ifndef MPEGTS_PACKET
#define MPEGTS_PACKET

#define TS_PACK_SIZE 188

void print_ts_packet(unsigned char* data, unsigned int bytes_per_line);

#endif