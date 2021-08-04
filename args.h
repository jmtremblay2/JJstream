#ifndef JJSTREAM_ARGS
#define JJSTREAM_ARGS

typedef struct jjstream_args{
    char* fname;
    int print_packets;
    int print_pids;
    int print_header;
    int print_pat_packets;
    int print_formatted_pat;
    unsigned int max_print;
    char* output_folder;
} jjstream_args;

jjstream_args get_args(int argc, char** argv);


#endif