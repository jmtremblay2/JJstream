#include "mpegts_packet.h"
#include "mpegts_pat.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>

// find the PMT and PAT and print them to a file
void discover_tables(mpegts_reader_data* rd, const char* output_folder){
    // files to print the tables
    char *pat_file_name = strcat_copy(output_folder, "/jjstream_pat.json");
    char *pmt_file_name = strcat_copy(output_folder, "/jjstream_pmt.json");

    mpegts_pat *pat = find_pat(rd);
    print_pat(pat, pat_file_name);

    // working with the assumption that there is only one program (program[0])
    uint16_t pmt_pid = pat->programs[0].data;
    mpegts_pmt *pmt = find_pmt(rd, pmt_pid);
    print_pmt(pmt, pmt_file_name);

    free(pmt_file_name);
    free(pat_file_name);
    return;
}