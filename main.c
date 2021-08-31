#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mpegts_header.h"
#include "mpegts_packet.h"
#include "mpegts_pat.h"
#include "args.h"
#include "features.h"

#define BUFFER_MAX 1000000
#define READ_MORE 500





int main(int argc, char** argv){
    jjstream_args args = get_args(argc, argv);

    mpegts_reader_data rd;
    ts_packet ts;
    init_mpegts_reader_data(args.fname, &rd, BUFFER_MAX, READ_MORE);

    if(strcmp(args.action, "discover") == 0){
        printf("discovering\n");
        discover_tables(&rd, args.output_folder);
    } else if(strcmp(args.action, "find_telemetry") == 0){
        printf("trying to find a telemetry\n");
        find_telemetry(&rd, args.output_folder, args.telemetry_pid);
    } else if(strcmp(args.action, "decode_telemetry") == 0){
        printf("trying to decode the telemetries");
        decode_telemetry(&rd, args.output_folder, args.telemetry_pid);
    }

    close_mpegts_reader_data(&rd);
    // uint16_t klv_pid = find_klv_pid(&rd, pmt);
    // klv_loop(&rd, klv_pid);
    // close_mpegts_reader_data(&rd);
    return 0;
}
   

