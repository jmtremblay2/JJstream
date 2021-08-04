#include "utils.h"

#include <string.h>
#include <stdlib.h>

uint32_t uint8_ptr_to_uint32_big_endian(uint8_t* c){
    uint32_t x;
    uint8_t* x_char = (uint8_t*) &x;
    x_char[3] = c[0];
    x_char[2] = c[1];
    x_char[1] = c[2];
    x_char[0] = c[3];
    return x;
}

int check_equal(double x1, double x2){
    double tol = 0.000000001;
    if ((x1 - x2) < -tol || (x1 - x2) > tol)
        return 0;
    return 1;
}

char* strcat_copy(const char* s1, const char* s2){
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    size_t l = l1 + l2 + 1;

    char* s = (char*) malloc(l * sizeof(char));
    memcpy(s, s1, l1);
    memcpy(s+l1, s2, l2);
    s[l-1] = '\0';

    return s;
}
