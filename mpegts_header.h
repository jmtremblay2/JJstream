#ifndef MPEGTS_HEADER
#define MPEGTS_HEADER

#include <stdint.h>

typedef struct mpegts_adaptation_extension{
    uint8_t adaptation_extension_length;
    uint8_t legal_time_window_flag;
    uint8_t piecewise_rate_flag;
    uint8_t seamless_splice_flag;
    uint8_t reserved1;
    // LTW
    uint8_t ltw_valid_flag;
    uint8_t ltw_offset[2];
    // Piecewise
    uint8_t reserved2;
    uint8_t piecewise_rate[3];
    // Seamless
    uint8_t splice_type;
    uint8_t dts_next_access_unit[5];
} mpegts_adaptation_extension;

typedef struct mpegts_adaptation{
    int has_adaptation;
    uint8_t adaptation_length;
    uint8_t discontinuity_indicator;
    uint8_t random_access_indicator;
    uint8_t priority_indicator;
    uint8_t pcr_flag;
    uint8_t opcr_flag;
    uint8_t splicing_point_flag;
    uint8_t transport_private_data_flag;
    uint8_t adaptation_field_extension_flag;

    uint8_t pcr[6];
    uint8_t opcr[6];
    uint8_t splice_countdown;
    uint8_t transport_private_data_length;
    uint8_t* transport_private_data;
    mpegts_adaptation_extension extension;

} mpegts_adaptation;
typedef struct mpegts_header {
    uint32_t sync;
    uint32_t tei;
    uint32_t pusi;
    uint32_t transport_priority;
    uint32_t pid;
    uint32_t tsc;
    uint32_t adaptation_field_control;
    uint32_t continuity_counter;

    mpegts_adaptation adaptation;
} mpegts_header;

uint32_t get_header_from_ts_packet(uint8_t* raw_data);
//void read_ts_packet_header(mpegts_header* h, uint8_t* raw_data);
void read_ts_packet_header(mpegts_header* h, uint8_t* x);
void print_ts_packet_header(mpegts_header* h);
void print_ts_packet_pid(mpegts_header* h);

void read_ts_packet_adaptation(mpegts_adaptation* a, 
    mpegts_header* h, uint8_t* ts_packet);

uint32_t ts_packet_adaptation_length(mpegts_adaptation* a);
void print_ts_packet_adaptation(mpegts_adaptation *a);

#endif