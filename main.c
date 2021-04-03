#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mpegts_header.h"
#include "mpegts_packet.h"
#include "mpegts_pat.h"
#include "args.h"

#define BUFFER_MAX 1000000
#define READ_MORE 500
#define PAT_PID 0

int main(int argc, char** argv){
    jjstream_args args = get_args(argc, argv);

    mpegts_reader_data rd;
    ts_packet ts;
    init_mpegts_reader_data(args.fname, &rd, BUFFER_MAX, READ_MORE);

    int printed_pat = 0, printed_pmt = 0;
    uint16_t pmt_pid=0xFFFF;
    uint16_t klv_pid=0xFFFF;
    while(has_next_ts_packet(&rd)){

        get_next_ts_packet(&rd, &ts);
        //print_ts_packet(&ts, 0, 0);        
        
        if(ts.h.pid == PAT_PID){
            //print_ts_packet(&ts, 0, 0);
            mpegts_pat* p = read_pat(&ts);
            if(! printed_pat){
                printed_pat = 1;
                printf("**I found the PAT:\n");
                print_pat(p);

                pmt_pid = p->programs[0].data;
                printf("**using pat PID: %x\n", pmt_pid);
            }
            delete_pat(p);
        }
        if(ts.h.pid == pmt_pid){
            if(! printed_pmt){
                printed_pmt = 1;
                printf("**I found the PMT:\n");
                //print_ts_packet(&ts, 0, 0);
                mpegts_pmt *pmt = read_pmt(&ts);
                print_pmt(pmt);
                delete_pmt(pmt);

                for(int i = 0; i < pmt->num_streams; ++i){
                    uint8_t stream_type = pmt->streams[i].stream_type;
                    if(stream_type == 6)
                        klv_pid = pmt->streams[i].elementary_pid;
                }

                printf("using the following pid for KLV: %x\n", klv_pid);
            }
        }
        if(ts.h.pid == klv_pid){
            printf("**I found the folowing telemetry:\n");
            print_ts_packet(&ts, 0, 0);
            break;
        }

    }
    close_mpegts_reader_data(&rd);
    return 0;
}

