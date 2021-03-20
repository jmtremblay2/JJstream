#ifndef MPEGTS_PAT
#define MPEGTS_PAT

// https://ecee.colorado.edu/~ecen5653/ecen5653/papers/iso13818-1.pdf
// page 61
typedef struct mpegts_pat_program{
    unsigned int program_number;
    unsigned int reserved;
    union  {
        unsigned int network_pid;
        unsigned int program_map_pid;
    };
} mpegts_pat_program;

typedef struct mpegts_pat {
    unsigned int table_id;
    unsigned int section_syntax_indicator;
    unsigned int zero;
    unsigned int reserved1;
    unsigned int section_length;
    unsigned int transport_stream_id;
    unsigned int reserved2;
    unsigned int version_number;
    unsigned int current_next_indicator;
    unsigned int section_number;
    unsigned int last_section_number;
    unsigned int num_sections;
    mpegts_pat_program* programs;
    unsigned crc_32;
} mpegts_pat;

mpegts_pat* create_pat_from_ts_packet(char* ts_packet);
void print_pat(mpegts_pat* p);
void delete_pat(mpegts_pat* p);

#endif