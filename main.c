#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mpegts_header.h"
#include "mpegts_packet.h"
#include "mpegts_pat.h"
#include "utils.h"

#define BUFFER_SIZE 10000000

int find_in_argv(char* s, int argc, char** argv){
    for(int i = 2; i < argc; ++i)
        if( 0 == strcmp(s, argv[i])){
            printf("found %s\n", s);
            return 1;
        }     
    return 0;
}

unsigned int find_int_in_argv(char* s, int argc, char** argv){
    for(int i = 2; i < argc; ++i)
        if( 0 == strcmp(s, argv[i])){
            printf("found %s\n", s);
            return (unsigned int) atoi(argv[i+1]);
        }     
    return 0x7FFFFFFF;
}


int main(int argc, char** argv){
    size_t bytes_read = 0;
    size_t packet_size = 188;
    size_t max_packets = 1000;

    char* fname = argv[1];//"/home/jm/Videos/video.ts"

    char mode = argv[2][0];
    int print_packets = find_in_argv("print_packets", argc, argv);
    int print_pids = find_in_argv("print_pids", argc, argv);
    int print_header = find_in_argv("print_header", argc, argv);
    int print_pat_packets = find_in_argv("print_pat_packets", argc, argv);
    int print_formatted_pat = find_in_argv("print_formatted_pat", argc, argv);

    unsigned int max_print = find_int_in_argv("max_print", argc, argv);
    //int print_header = find_in_argv("print_header", argc, argv);

    FILE* f = fopen(fname,"r");
    unsigned char buffer[max_packets * packet_size];
    mpegts_header h;
    unsigned int counter = 0;
    do {
        // read packets
        bytes_read = fread(buffer, sizeof(unsigned char), max_packets * packet_size, f);
        double packets_read = ((double) bytes_read) / packet_size;
        int packets_read_int = bytes_read / packet_size;

        for(int i = 0; i < packets_read_int; ++i){
            counter++;
            read_ts_packet_header(&h, buffer +(188*i));

            if(print_packets)
                print_ts_packet(buffer +(188*i), 188);
            if(print_header)
                print_ts_packet_header(&h);
            if(print_pids)    
                printf("%x\n", h.pid);

            if(0 == h.pid){
                if(print_pat_packets)
                    print_ts_packet(buffer +(188*i), 188);
                mpegts_pat *p = create_pat_from_ts_packet(buffer+(188*i));
                if(print_formatted_pat)
                    print_pat(p);
                delete_pat(p);
            }
        }
        if(counter > max_print) break;
    } while(0 < bytes_read);


    // fclose(f);
    // return 0;
}

