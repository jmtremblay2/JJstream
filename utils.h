#ifndef MPEGTS_UTILS
#define MPEGTS_UTILS

#include <stdint.h>

uint32_t uint8_ptr_to_uint32_big_endian(uint8_t* c);
int check_equal(double x1, double x2);
char* strcat_copy(const char* s1, const char* s2);

#endif