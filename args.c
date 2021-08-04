#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"

char *home_folder = "/home/jm";

static int find_in_argv(char* s, int argc, char** argv){
    for(int i = 2; i < argc; ++i)
        if( 0 == strcmp(s, argv[i])){
            //printf("found %s\n", s);
            return 1;
        }     
    return 0;
}

static unsigned int find_int_in_argv(char* s, int argc, char** argv){
    for(int i = 2; i < argc; ++i)
        if( 0 == strcmp(s, argv[i])){
            //printf("found %s\n", s);
            return (unsigned int) atoi(argv[i+1]);
        }     
    return 0x7FFFFFFF;
}

static char* find_str_in_argv(char* s, int argc, char** argv){
    for(int i = 2; i < argc; ++i)
        if( 0 == strcmp(s, argv[i])){
            //printf("found %s\n", s);
            return argv[i+1];
        }     
    return home_folder;
}

jjstream_args get_args(int argc, char** argv){
    jjstream_args args;
    args.fname = argv[1];
    args.print_packets = find_in_argv("print_packets", argc, argv);
    args.print_pids = find_in_argv("print_pids", argc, argv);
    args.print_header = find_in_argv("print_header", argc, argv);
    args.print_pat_packets = find_in_argv("print_pat_packets", argc, argv);
    args.print_formatted_pat = find_in_argv("print_formatted_pat", argc, argv);
    args.max_print = find_int_in_argv("max_print", argc, argv);
    args.output_folder = find_str_in_argv("output_folder", argc, argv);
    return args;
}

