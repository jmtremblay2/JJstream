#include <stdio.h>

#include "mpegts_header.h"
#include "utils.h"

static unsigned int mpegts_header_sync_mask               = 0xFF000000;
static unsigned int mpegts_header_tei_mask                = 0x00800000;
static unsigned int mpegts_header_pusi_mask               = 0x00400000;
static unsigned int mpegts_header_transport_priority_mask = 0x00200000;
static unsigned int mpegts_header_pid_mask                = 0x001FFF00;
static unsigned int mpegts_header_tsc_mask                = 0x000000C0;
static unsigned int mpegts_header_adaptation_mask         = 0x00000030;
static unsigned int mpegts_header_continuity_counter_mask = 0x0000000f;

static unsigned int mpegts_header_sync_shift               = 24;
static unsigned int mpegts_header_tei_shift                = 23;
static unsigned int mpegts_header_pusi_shift               = 22;
static unsigned int mpegts_header_transport_priority_shift = 21;
static unsigned int mpegts_header_pid_shift                = 8;
static unsigned int mpegts_header_tsc_shift                = 6;
static unsigned int mpegts_header_adaptation_shift         = 4;
static unsigned int mpegts_header_continuity_counter_shift = 0;

unsigned int get_header_from_ts(char* ts_packet){
    return charptr_to_unsigned_int_big_endian(ts_packet);
    // unsigned int x;
    // char* c = (char*) &x;
    // // #if __BYTE_ORDER == __BIG_ENDIAN
    // // //#if IS_BIG_ENDIAN
    //     // printf("big endian!!\n");
    //     c[3] = ts_packet[0];
    //     c[2] = ts_packet[1];
    //     c[1] = ts_packet[2];
    //     c[0] = ts_packet[3];
    // // #else
    // //      printf("little endian!!\n");
    // // unsigned int y;
    // // char* c2 = (char*) &y;
    //     // c[0] = ts_packet[0];
    //     // c[1] = ts_packet[1];
    //     // c[2] = ts_packet[2];
    //     // c[3] = ts_packet[3];

    // // printf("x: %x\n",x);
    // // printf("y: %x\n",y);

    // // #endif


    // return x;
}

void read_ts_packet_header(mpegts_header* h, char* ts_packet){
    unsigned int x = get_header_from_ts(ts_packet);

    h->sync = (x & mpegts_header_sync_mask) >> 
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
    h->adaptation = (x & mpegts_header_adaptation_mask) >> 
        mpegts_header_adaptation_shift;
    h->continuity_counter = (x & mpegts_header_continuity_counter_mask) >> 
        mpegts_header_continuity_counter_shift;

    return;
}

void print_ts_packet_header(mpegts_header* h){
    printf("sync: %x\n",h->sync);
    printf("tei: %x\n", h->tei);
    printf("pusi: %x\n", h->pusi);
    printf("transport priority: %x\n", h->transport_priority);
    printf("pid: %x\n", h->pid);
    printf("tsc: %x\n", h->tsc);
    printf("adaptation: %x\n", h->adaptation);
    printf("continuity: %x\n", h->continuity_counter);    
}

void print_ts_packet_pid(mpegts_header* h){
    printf("pid: %x\n", h->pid);
}
