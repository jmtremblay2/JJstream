#include <stdio.h>
#include <stdint.h>

#include "mpegts_header.h"
#include "utils.h"

static uint32_t mpegts_header_sync_mask               = 0xFF000000;
static uint32_t mpegts_header_tei_mask                = 0x00800000;
static uint32_t mpegts_header_pusi_mask               = 0x00400000;
static uint32_t mpegts_header_transport_priority_mask = 0x00200000;
static uint32_t mpegts_header_pid_mask                = 0x001FFF00;
static uint32_t mpegts_header_tsc_mask                = 0x000000C0;
static uint32_t mpegts_header_adaptation_mask         = 0x00000030;
static uint32_t mpegts_header_continuity_counter_mask = 0x0000000f;

static uint32_t mpegts_header_sync_shift               = 24;
static uint32_t mpegts_header_tei_shift                = 23;
static uint32_t mpegts_header_pusi_shift               = 22;
static uint32_t mpegts_header_transport_priority_shift = 21;
static uint32_t mpegts_header_pid_shift                = 8;
static uint32_t mpegts_header_tsc_shift                = 6;
static uint32_t mpegts_header_adaptation_shift         = 4;
static uint32_t mpegts_header_continuity_counter_shift = 0;

uint32_t get_header_from_ts_packet(uint8_t* raw_data){
    return uint8_ptr_to_uint32_big_endian(raw_data);
}

//void read_ts_packet_header(mpegts_header* h, uint8_t* raw_data){
void read_ts_packet_header(mpegts_header* h, uint8_t* packet_data){
    uint32_t x = uint8_ptr_to_uint32_big_endian(packet_data);

    h->sync = (x & (uint32_t) mpegts_header_sync_mask) >> 
        mpegts_header_sync_shift;
    h->tei = (x & mpegts_header_tei_mask) >> 
        mpegts_header_tei_shift;
    h->pusi = (x & mpegts_header_pusi_mask) >> 
        mpegts_header_pusi_shift;
    h->transport_priority  = (x & mpegts_header_transport_priority_mask) >> 
        mpegts_header_tei_shift;
    h->pid = (x & mpegts_header_pid_mask) >> 
        mpegts_header_pid_shift;
    h->tsc = (x & mpegts_header_tsc_mask) >> 
        mpegts_header_tsc_shift;
    h->adaptation_field_control = (x & mpegts_header_adaptation_mask) >> 
        mpegts_header_adaptation_shift;
    h->continuity_counter = (x & mpegts_header_continuity_counter_mask) >> 
        mpegts_header_continuity_counter_shift;

    // the adaptation length fields itself packet_data[4] takes a byte
    h->adaptation.adaptation_length = 1 + packet_data[4];
    return;
}

void print_ts_packet_header(mpegts_header* h){

    printf("sync: %x\n",h->sync);
    printf("tei: %x\n", h->tei);
    printf("pusi: %x\n", h->pusi);
    printf("transport priority: %x\n", h->transport_priority);
    printf("pid: %x\n", h->pid);
    printf("tsc: %x\n", h->tsc);
    printf("adaptation: %x\n", h->adaptation_field_control);
    printf("continuity: %x\n", h->continuity_counter);    
}

void print_ts_packet_pid(mpegts_header* h){
    printf("pid: %x\n", h->pid);
}

void read_ts_packet_adaptation(mpegts_adaptation* a, mpegts_header* h, uint8_t *ts_packet){
    // check if there is any adaptation
    // if the adaptation bits are 01 (value of 1) there is no adaptation field
    if(h->adaptation_field_control == 1){
        a->has_adaptation = 0;
        a->adaptation_length = 1;
        return;
    }

    unsigned int current_index = 1;
    char adaptation_field = ts_packet[current_index];
    a->discontinuity_indicator         = (adaptation_field & 0x80) && 1;
    a->random_access_indicator         = (adaptation_field & 0x40) && 1;
    a->priority_indicator              = (adaptation_field & 0x20) && 1;
    a->pcr_flag                        = (adaptation_field & 0x10) && 1;
    a->opcr_flag                       = (adaptation_field & 0x08) && 1;
    a->splicing_point_flag             = (adaptation_field & 0x04) && 1;
    a->transport_private_data_flag     = (adaptation_field & 0x02) && 1;
    a->adaptation_field_extension_flag = (adaptation_field & 0x01) && 1;
    current_index++;

    if(a->pcr_flag){
        // for(int j = 0; j < 6; ++j){
        //     a->pcr[j] = ts_packet[current_index];
        //     current_index++;
        // }
        current_index += 6;
    }
    if(a->opcr_flag){
        // for(int j = 0; j < 6; ++j){
        //     a->opcr[j] = ts_packet[current_index];
        //     current_index++;
        // }
        current_index += 6;
    }
    if(a->transport_private_data_flag){
        a->transport_private_data_length = ts_packet[current_index];
        current_index++;
        // currently don't read that field, but fast forward the pointer to account for it
        current_index += a->transport_private_data_length;
        // a->transport_private_data = (char*) malloc(sizeof(char) * a->transport_private_data_length);
        // for(unsigned char j = 0; j < a->transport_private_data_length; ++j)
        //     a->transport_private_data[j] = ts_packet[current_index++];
    }
    if(a->adaptation_field_extension_flag){
        a->extension.adaptation_extension_length = ts_packet[current_index++];
        char extension_second_byte = ts_packet[current_index++];
        a->extension.legal_time_window_flag = (extension_second_byte & 0x80) && 1;
        a->extension.piecewise_rate_flag    = (extension_second_byte & 0x40) && 1;
        a->extension.seamless_splice_flag   = (extension_second_byte & 0x20) && 1;

        if(a->extension.legal_time_window_flag) current_index += 2;
        if(a->extension.piecewise_rate_flag) current_index += 3;
        if(a->extension.seamless_splice_flag) current_index += 5;
    }
    a->adaptation_length = current_index;
}

unsigned int ts_packet_adaptation_length(mpegts_adaptation* a){
    return a->adaptation_length;
}

void print_ts_packet_adaptation(mpegts_adaptation *a){
    if(a->has_adaptation == 1){
        printf("no adaptation\n");
    }
    return ;
    // discontinuity_indicator;
    // random_access_indicator;
    // priority_indicator;
    // pcr_flag;
    // opcr_flag;
    // splicing_point_flag;
    // transport_private_data_flag;
    // adaptation_field_extension_flag;

    // char pcr[6];
    // char opcr[6];
    // char splice_countdown;
    // unsigned char transport_private_data_length;
    // char* transport_private_data;
    // mpegts_adaptation_extension extension;
}
