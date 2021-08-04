#ifndef MPEGTS_PAT
#define MPEGTS_PAT

#include "mpegts_packet.h"
#define PAT_PID 0

// https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf
// page 61
typedef struct mpegts_pat_program{
    uint16_t program_number;
    unsigned int reserved;
    // union  {
    //     uint16_t network_pid;
    //     uint16_t program_map_pid;
    // };
    uint16_t data;
} mpegts_pat_program;

typedef struct mpegts_pat {
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint8_t zero;
    uint8_t reserved1;
    uint16_t section_length;
    int16_t transport_stream_id;
    uint8_t reserved2;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    size_t num_sections;
    mpegts_pat_program* programs;
    uint32_t crc_32;
} mpegts_pat;

mpegts_pat* create_pat_from_ts_packet(char* ts_packet);
mpegts_pat* read_pat(ts_packet* ts);
void print_pat(mpegts_pat* p, const char* output_file_name);
void delete_pat(mpegts_pat* p);

typedef struct mpegts_stream{
    uint8_t stream_type;
    uint16_t elementary_pid;
    uint16_t es_info_length;
    char* descriptor;
    // es info for now we don't save it;
} mpegts_stream; 

typedef struct mpegts_pmt{
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    // zero
    // reserved
    uint16_t section_length;
    uint16_t program_number;
    //reserved
    uint8_t version;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    // reserved
    uint16_t pcr_pid;
    //reserved
    uint16_t program_info_length;
    size_t num_streams;
    size_t num_streams_allocated;
    mpegts_stream* streams;
    uint32_t crc_32;
} mpegts_pmt;

mpegts_pmt* read_pmt(ts_packet* ts);
void print_pmt(mpegts_pmt* p, const char *output_file_name);
void delete_pmt(mpegts_pmt* p);

mpegts_pat* find_pat(mpegts_reader_data* rd);
mpegts_pmt* find_pmt(mpegts_reader_data* rd, uint16_t pmt_pid);


#endif