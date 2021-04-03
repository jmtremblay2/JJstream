#include <stdlib.h>
#include <stdio.h>

#include "mpegts_pat.h"
#include "mpegts_header.h"
#include "utils.h"


mpegts_pat* read_pat(ts_packet* ts){
//mpegts_pat* create_pat_from_ts_packet(char* ts_packet){
    // unsigned int x = get_header_from_ts(ts_packet);
    // mpegts_header h;
    // read_ts_packet_header(&h, ts_packet);
    if(ts->h.pid != 0){
        fprintf(stderr, "ts packet does not have PID of 0, corresponding to PAT\n");
        return NULL;
        }
    mpegts_pat *p = (mpegts_pat*) malloc(sizeof(mpegts_pat));

    // everything we read is after the header, and after the adaptation so we always 
    // add 4 + adaptation length
    uint8_t* data = ts->payload;
    // table id, first 8 bits;
    p->table_id = data[0];
    p->section_syntax_indicator = ((data[1] & 0x80) >> 7);
    p->zero = ((data[1] & 0x40)) >> 6;
    // // reserved 2 bits
    // // section length is 12 bits long, and starts after bit 12
    p->section_length = ((uint16_t) ((data[1] & 0x0F) << 8)) + ((uint16_t) data[2]);

    // // ts id are bytes 3,4 from the beginning of the payload
    p->transport_stream_id = (uint16_t) (uint8_ptr_to_uint32_big_endian(data + 3) >> 16);

    // // 2nd reserved field = first 2 bits of byte 5
    // p->reserved2 = (unsigned int) ((ts_packet[9] & 0xC0) >> 6);

    // version number = bits 3 to 7 of byte 5
    p->version_number = (data[5] & 0x3E) >> 1;

    // // current next indicator = last bit of byte 5;
    p->current_next_indicator = data[5] & 0x01;

    // // section number is byte 6,, last section is byte 7;
    p->section_number = data[6];
    p->last_section_number = data[7];

    p->num_sections = (p->section_length - 9) / 4;
    p->programs = (mpegts_pat_program*) malloc(p->num_sections * sizeof(mpegts_pat_program));
    for(int i = 0; i < p->num_sections; ++i){
        uint8_t *this_program = data + 8 + 4*i;
        p->programs[i].program_number = (((uint16_t) this_program[0]) << 8) + this_program[1];
        uint16_t program_data = ((((uint16_t) this_program[2]) << 8) + this_program[3]) & 0x1fff;
        p->programs[i].data = program_data;
    }
    p->crc_32 = uint8_ptr_to_uint32_big_endian(data + p->section_length -1);
    return p;
}

void print_pat(mpegts_pat* p){
    printf("table_id: %d\n", p->table_id);
    printf("section_syntax: %x\n", p->section_syntax_indicator);
    //printf("zero: %x\n", p->zero);
    printf("section_length: %d\n", p->section_length);
    printf("transport stream id: %d\n", p->transport_stream_id);
    printf("version number: %d\n",p->version_number);
    printf("current next indicator: %d\n",p->current_next_indicator);

    printf("section number: %d\n", p->section_number);
    printf("last section number: %d\n", p->last_section_number);
    printf("programs:\n");
    for(int i = 0; i < p->num_sections; ++i){
        if(p->programs[i].program_number == 0)
            printf("-->network pid: %d\n", p->programs[i].data);
        else
            printf("-->program map pid: %d\n", p->programs[i].data);
    }
    printf("crc: %x\n", p->crc_32);
    return;
}

void delete_pat(mpegts_pat* p){
    if(p && p->programs)
        free(p->programs);
    if(p)
        free(p);
    return;
}

mpegts_pmt* read_pmt(ts_packet* ts){
//mpegts_pat* create_pat_from_ts_packet(char* ts_packet){
    mpegts_pmt *pmt = (mpegts_pmt*) malloc(sizeof(mpegts_pmt));
    // everything we read is after the header, and after the adaptation so we always 
    // add 4 + adaptation length
    uint8_t* data = ts->payload;
    // table id, first 8 bits;
    pmt->table_id = data[0];
    pmt->section_syntax_indicator = ((data[1] & 0x80) >> 7);
    //pmt->zero = ((data[1] & 0x40)) >> 6;
    // // reserved 2 bits
    // // section length is 12 bits long, and starts after bit 12
    pmt->section_length = ((uint16_t) ((data[1] & 0x0F) << 8)) + ((uint16_t) data[2]);

    // // program number is bytes 3,4 from the beginning of the payload
    pmt->program_number = (uint16_t) (uint8_ptr_to_uint32_big_endian(data + 3) >> 16);

    // // 2nd reserved field = first 2 bits of byte 5
    // p->reserved2 = (unsigned int) ((ts_packet[9] & 0xC0) >> 6);

    // version number = bits 3 to 7 of byte 5
    pmt->version = (data[5] & 0x3E) >> 1;

    // // current next indicator = last bit of byte 5;
    pmt->current_next_indicator = data[5] & 0x01;

    // // section number is byte 6,, last section is byte 7;
    pmt->section_number = data[6];
    pmt->last_section_number = data[7];

    pmt->pcr_pid = ((uint16_t) ((data[8] & 0x1F) << 8)) + ((uint16_t) data[9]);
    //pmt->program_info_length = ((uint16_t) ((data[10] & 0x0F) << 8)) + ((uint16_t) data[11]);
    // first two bits of info length are zero so mask them out here
    pmt->program_info_length = ((uint16_t) ((data[10] & 0x03) << 8)) + ((uint16_t) data[11]);
    
    int end_of_packet=0;
    int i=0;
    uint8_t *this_stream= data + 12 + pmt->program_info_length;
    uint8_t *address_of_crc = data + 3 + pmt->section_length - 4;
    size_t streams_allocated = 10;
    pmt->streams = (mpegts_stream*) malloc(sizeof(mpegts_stream) * streams_allocated);
    pmt->num_streams_allocated = streams_allocated;
    pmt->num_streams = 0;
    while(!end_of_packet){
        if(i > pmt->num_streams_allocated){
            fprintf(stderr,"error parsing program map table, not enough memory allocated\n");
            return NULL;
        }
        pmt->streams[i].stream_type = this_stream[0];
        pmt->streams[i].elementary_pid =  ((uint16_t) ((this_stream[1] & 0x1F) << 8)) + ((uint16_t) this_stream[2]);
        pmt->streams[i].es_info_length = ((uint16_t) ((this_stream[3] & 0x03) << 8)) + ((uint16_t) this_stream[4]);

        pmt->streams[i].descriptor = (char*) malloc(pmt->streams[i].es_info_length + 1);
        for(int k = 0; k < pmt->streams[i].es_info_length; ++k)
            pmt->streams[i].descriptor[k] = this_stream[k + 4];
        pmt->streams[i].descriptor[pmt->streams[i].es_info_length] = '\0';

        this_stream += (5 + pmt->streams[i].es_info_length);
        end_of_packet = this_stream == address_of_crc;
        pmt->num_streams++;
        i++;
    }
    pmt->crc_32 = uint8_ptr_to_uint32_big_endian(address_of_crc);
    return pmt;
}
void print_pmt(mpegts_pmt* pmt){
    printf("table_id: %d\n", pmt->table_id);
    printf("section_syntax: %x\n", pmt->section_syntax_indicator);
    //printf("zero: %x\n", p->zero);
    printf("section_length: %d\n", pmt->section_length);
    printf("program number: %d\n", pmt->program_number);
    printf("version number: %d\n",pmt->version);
    printf("current next indicator: %d\n",pmt->current_next_indicator);

    printf("section number: %d\n", pmt->section_number);
    printf("last section number: %d\n", pmt->last_section_number);
    printf("pcr pid: %d\n", pmt->pcr_pid);
    
    printf("streams:\n");
    for(int i = 0; i < pmt->num_streams; ++i){
        printf("-->streams[%d]",i);
        printf("---->stream type: %d\n", pmt->streams[i].stream_type);
        printf("---->elementary pid: %x\n", pmt->streams[i].elementary_pid);
        printf("---->stream info length: %d\n", pmt->streams[i].es_info_length);
        printf("---->stream descriptor: %s\n", pmt->streams[i].descriptor);
    }
    printf("crc: %x\n", pmt->crc_32);
    return;

}
void delete_pmt(mpegts_pmt* pmt){
    if(pmt && pmt->streams){
        for(int i = 0; i < pmt->num_streams_allocated; ++i){
            if(pmt->streams[i].descriptor)
                free(pmt->streams[i].descriptor);
        }
        free(pmt->streams);
    }
    if(pmt)
        free(pmt);
    return;
}