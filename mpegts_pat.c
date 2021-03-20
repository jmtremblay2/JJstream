#include <stdlib.h>
#include <stdio.h>

#include "mpegts_pat.h"
#include "mpegts_header.h"
#include "utils.h"

mpegts_pat* create_pat_from_ts_packet(char* ts_packet){
    unsigned int x = get_header_from_ts(ts_packet);
    mpegts_header h;
    read_ts_packet_header(&h, ts_packet);
    if(h.pid != 0){
        fprintf(stderr, "ts packet does not have PID of 0, corresponding to PAT\n");
        return NULL;
        }
    mpegts_pat *p = (mpegts_pat*) malloc(sizeof(mpegts_pat));

    // everything we read is after the header, so we always 
    // add 4 to ts_packet

    // table id, first 8 bits;
    p->table_id = (unsigned int) (ts_packet[4]);
    p->section_syntax_indicator = (unsigned int) ((ts_packet[5] & 0x80) >> 7);
    p->zero = (unsigned int) ((ts_packet[5] & 0x40)) >> 6;
    // reserved 2 bits
    // section length is 12 bits long, and starts after bit 12
    // so bits 13 through 24 after the header
    unsigned int x_section_length = charptr_to_unsigned_int_big_endian(ts_packet + 5);
    p->section_length = (x_section_length & 0x0FFF0000) >> 16;

    // ts is are bytes 7 and 8 from the beginning of the packet
    p->transport_stream_id = charptr_to_unsigned_int_big_endian(ts_packet + 7) >> 16;

    // 2nd reserved field = first 2 bits of byte 9
    p->reserved2 = (unsigned int) ((ts_packet[9] & 0xC0) >> 6);

    // version number = bits 3 to 7 of byte 9
    p->version_number = (unsigned int) ((ts_packet[9] & 0x3E) >> 1);

    // current next indicator = last bit of byte 9;
    p->current_next_indicator = (unsigned int) ts_packet[9] & 0x01;

    // section number is byte 10, last section is byte 11;
    p->section_number = ((unsigned int) ts_packet[10]) & 0xFF;
    p->last_section_number = (unsigned int) ts_packet[11] & 0xFF;

    // 

    return p;
}

void print_pat(mpegts_pat* p){
    printf("table_id: %x\n", p->table_id);
    printf("section_syntax: %x\n", p->section_syntax_indicator);
    printf("zero: %x\n", p->zero);
    printf("section_length: %x\n", p->section_length);
    printf("transport stream id: %x\n", p->transport_stream_id);
    printf("version number: %x\n",p->version_number);
    printf("current next indicator: %x\n",p->current_next_indicator);

    printf("section number: %x\n", p->section_number);
    printf("last section number: %x\n", p->last_section_number);

    return;
}

void delete_pat(mpegts_pat* p){
    if(p && p->programs)
        free(p->programs);
    if(p)
        free(p);
    return;
}
